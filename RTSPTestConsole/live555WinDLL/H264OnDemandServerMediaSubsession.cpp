#include "H264OnDemandServerMediaSubsession.h"

H264OnDemandServerMediaSubsession * H264OnDemandServerMediaSubsession::createNew(UsageEnvironment & env, Boolean reuseFirstSource)
{
	
	return new H264OnDemandServerMediaSubsession(env, reuseFirstSource);
}

H264OnDemandServerMediaSubsession::H264OnDemandServerMediaSubsession(UsageEnvironment & env, Boolean reuseFirstSource) : 
	OnDemandServerMediaSubsession(env, reuseFirstSource)
{
	bDeviceFinished = true;
	bDataPresent = false;
}

H264OnDemandServerMediaSubsession::~H264OnDemandServerMediaSubsession()
{
	envir() << "~H264OnDemandServerMediaSubsession\n";
}
static void checkForAuxSDPLine(void* clientData) {
	H264OnDemandServerMediaSubsession* subsess = (H264OnDemandServerMediaSubsession*)clientData;
	subsess->checkForAuxSDPLine1();
}

static void afterPlayingDummy(void* clientData)
{
	((H264OnDemandServerMediaSubsession*)clientData)->afterPlayingDummy1();
}
void H264OnDemandServerMediaSubsession::afterPlayingDummy1()
{
	// Unschedule any pending 'checking' task:
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	// Signal the event loop that we're done:
	setDoneFlag();
}
char const* H264OnDemandServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
	if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

	if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
								 // Note: For H264 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
								 // until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
								 // and we need to start reading data from our file until this changes.
		//auto env = envir();
		fDummyRTPSink = rtpSink;
		
		envir() << "[H264OnDemandServerMediaSubsession::getAuxSDPLine] start reading from injected frames\n";
		// Start reading the file:
		fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);

		// Check whether the sink's 'auxSDPLine()' is ready:
		checkForAuxSDPLine(this);
	}

	envir().taskScheduler().doEventLoop(&fDoneFlag);

	return fAuxSDPLine;
}

void H264OnDemandServerMediaSubsession::checkForAuxSDPLine1() {
	char const* dasl;

	if (fAuxSDPLine != NULL) {
		// Signal the event loop that we're done:
		setDoneFlag();
	}
	else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
		fAuxSDPLine = strDup(dasl);
		fDummyRTPSink = NULL;

		// Signal the event loop that we're done:
		setDoneFlag();
	}
	else if (!fDoneFlag) {
		// try again after a brief delay:
		int uSecsToDelay = 100000; // 100 ms
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			(TaskFunc*)checkForAuxSDPLine, this);
	}
}
bool isPSC(unsigned char*  buf, int offset)
{
	return (buf[offset] == 0 && buf[offset + 1] == 0 && buf[offset + 2] == 0 && buf[offset + 3] == 1);
}
bool GetNextNALOffset(unsigned char* buf, int szBuf, int& offsetPSCs, int& type)
{
	bool bFoundPSC = false;
	type = 0;
	while (!bFoundPSC && (offsetPSCs < (szBuf - 4)))
	{
		bFoundPSC = isPSC(buf, offsetPSCs);
		if (!bFoundPSC)
			offsetPSCs++;
		else
			type = buf[offsetPSCs + 4] & 0x1F;
	}
	return bFoundPSC;
}
void H264OnDemandServerMediaSubsession::CopyHeaders(unsigned char* buf, unsigned int szBuf, int offsetPSCs, int lastOffset, int lastNALType)
{
	int NALSize = offsetPSCs - lastOffset;
	if (lastNALType == 0x7)
	{
		std::vector<unsigned char> _sps(&buf[lastOffset], &buf[lastOffset + NALSize]);
		sps = std::move(_sps);
	}
	if (lastNALType == 0x8)
	{
		std::vector<unsigned char> _pps(&buf[lastOffset], &buf[lastOffset + NALSize]);
		pps = std::move(_pps);
	}
}
bool H264OnDemandServerMediaSubsession::FillHeaders(unsigned char*& buf, unsigned int& szBuf, int& offsetPSCs)
{
	int NALType = 0;
	int lastNALType = NALType;
	bool bFoundFrame = false;
	int lastOffset = offsetPSCs;
	while (GetNextNALOffset(buf, szBuf, offsetPSCs, NALType) == true)
	{
		CopyHeaders(buf, szBuf, offsetPSCs, lastOffset, lastNALType);
		lastOffset = offsetPSCs;
		lastNALType = NALType;
		if (NALType == 0x5)
		{
			bFoundFrame = true;
			/*if (sps.Length > 4 && pps.Length > 4)
			{
				int newSize = sps.Length + pps.Length + buf.Length - lastOffset;
				byte[] bufWithHeaders = new byte[newSize];
				Array.Copy(sps, 0, bufWithHeaders, 0, sps.Length);
				Array.Copy(pps, 0, bufWithHeaders, sps.Length, pps.Length);
				Array.Copy(buf, lastOffset, bufWithHeaders, sps.Length + pps.Length, buf.Length - lastOffset);
				buf = bufWithHeaders;
			}*/
			break;
		}
		if (NALType == 0x1)
		{
			bFoundFrame = true;
			break;
		}
		offsetPSCs++;
	}
	CopyHeaders(buf, szBuf, bFoundFrame ? offsetPSCs : szBuf, lastOffset, lastNALType);
	return bFoundFrame;
}
void H264OnDemandServerMediaSubsession::InjectFrame(unsigned char* buf, unsigned int bufSize)
{
	int offsetPSC = 0;
	FillHeaders(buf, bufSize, offsetPSC);
	if (m_pDevSource && !bDeviceFinished)
	{
		std::lock_guard<std::mutex> lk(m_pDevSource->m);
		if (bDeviceFinished)
		{
			
		}
		else
		{
			m_pDevSource->bInjecting = true;
			m_pDevSource->InjectFrame(buf, bufSize);
			if (bDataPresent == false)
			{
				envir() << "H264OnDemandServerMediaSubsession::InjectFrame Data present\n";
			}
			bDataPresent = true;
			cvNodata.notify_one();

			m_pDevSource->bInjecting = false;
			m_pDevSource->cv.notify_all();
		}
	}
	if (bDeviceFinished)
	{
		m_pDevSource = nullptr;
	}
	
}

FramedSource * H264OnDemandServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned & estBitrate)
{
	DeviceParameters devParams;
	auto env = &envir();
	
	m_pDevSource = new DeviceSource(*env, devParams);
	
	if (m_pDevSource == NULL)
	{

		*env << "Unable to read from\"" << "Buffer"
			<< "\" as a byte-stream source\n";
		exit(1);
	}
	bDeviceFinished = false;
	m_pDevSource->SetCallback([&]() {bDeviceFinished = true; });
	//std::unique_lock<std::mutex> l(m);
	//cvNodata.wait(l, [this]() {return (bDataPresent == true); });

	FramedSource* videoES(&*m_pDevSource);

	// Create a framer for the Video Elementary Stream:
	//videoSource = H264VideoStreamFramer::createNew(*env, videoES, False);
	videoSource = H264VideoStreamDiscreteFramer::createNew(*env, videoES);
	envir() << "H264OnDemandServerMediaSubsession::createNewStreamSource Source created\n";
	return videoSource;

	// Finally, start playing:
	//*env << "Beginning to read from UPP...\n";
	//videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}

RTPSink * H264OnDemandServerMediaSubsession::createNewRTPSink(Groupsock * rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource * inputSource)
{
	OutPacketBuffer::maxSize = 1500000;
	auto sink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, &sps[0], sps.size(), &pps[0], pps.size());
	//sink->setPacketSizes(1000, 500000);
	return sink;
}
