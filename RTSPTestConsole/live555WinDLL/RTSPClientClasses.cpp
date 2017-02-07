#include "RTSPClientClasses.h"
// Implementation of "ourRTSPClient":
#include "MediaSession.hh"
#include <mutex>
ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
	int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
	return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
	int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
	: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) {
}

ourRTSPClient::~ourRTSPClient() {
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
	: iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
	delete iter;
	if (session != NULL) {
		// We also need to delete "session", and unschedule "streamTimerTask" (if set)
		UsageEnvironment& env = session->envir(); // alias

		env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
		Medium::close(session);
	}
}


// Implementation of "DummySink":

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 1500000

DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, std::function< FrameBuf() > getFrameCallback, char const* streamId) {
	return new DummySink(env, subsession, getFrameCallback, streamId);
}

DummySink::FrameBuf DummySink::GetFrame(bool bCopy)
{
	std::lock_guard<std::mutex> locker(frameBuffersMutex);
	std::string sCurrFrameBuf;
	if (currFrameBuf == nullptr)
	{
		sCurrFrameBuf = "null";
	}
	else
	{
		char temp[16];
		sprintf(temp, "%d", currFrameBuf->size());
		sCurrFrameBuf = temp;
	}
	char temp[16];
	sprintf(temp, "%d", frameBuffers.size());
	envir() << "DummySink::GetFrame frameBuffers " << temp << " ,currFrameBuf size " << sCurrFrameBuf.c_str() << " \n";
	if (frameBuffers.empty())
	{
		return nullptr;

		//if (currFrameBuf == nullptr || currFrameBuf->empty())
		//{
		//	return nullptr;
		//}
		//else
		//{
		//	FrameBuf frameBuf;
		//	if (bCopy)
		//	{
		//		frameBuf = std::make_shared< std::vector<unsigned char >>(&((*currFrameBuf)[0]), &((*currFrameBuf)[currFrameBuf->size() - 1]));
		//	}
		//	else
		//	{
		//		frameBuf = std::move(currFrameBuf);
		//	}
		//	return frameBuf;
		//}
	}
	else
	{
		{
			currFrameBuf = frameBuffers.front();
			frameBuffers.pop();
			
		}
		FrameBuf frameBuf;
		if (bCopy)
		{
			frameBuf = std::make_shared< std::vector<unsigned char >>(&((*currFrameBuf)[0]), &((*currFrameBuf)[currFrameBuf->size() - 1]));
		}
		else
		{
			frameBuf = std::move(currFrameBuf);
		}
		return frameBuf;
	}
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, std::function< FrameBuf() > getFrameCallback, char const* streamId)
	: MediaSink(env),
	fSubsession(subsession), m_env(env){
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
	m_callback = getFrameCallback;
}

DummySink::~DummySink() {
	delete[] fReceiveBuffer;
	delete[] fStreamId;
}

//int DummySink::GetFrame(unsigned char** buf, const unsigned allocSize, unsigned* bufSize, bool bCopy)
//{
//	if (frameBuffers.empty())
//	{
//		if (currFrameBuf == nullptr)
//		{
//			*buf = nullptr;
//			*bufSize = 0;
//			return -1;
//		}
//		else if (currFrameBuf->empty())
//		{
//			*buf = nullptr;
//			*bufSize = 0;
//			return -1;
//		}
//		else
//		{
//			if (bCopy)
//			{
//				memcpy_s(*buf, allocSize, &((*currFrameBuf)[0]), currFrameBuf->size());
//			}
//			else
//			{
//				*buf = fFullFrame;
//			}
//		
//		}
//
//	}
//	else
//	{
//
//		currFrameBuf = frameBuffers[0];
//		frameBuffers.pop_back();
//		if (bCopy)
//		{
//			memcpy_s(*buf, allocSize, &((*currFrameBuf)[0]), currFrameBuf->size());
//		}
//		else
//		{
//			*buf = &((*currFrameBuf)[0]);
//		}
//	}
//	*bufSize = currFrameBuf->size();
//	FILE* fStream;
//	auto err = fopen_s(&fStream, "C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\testOut.264", "ab");
//	size_t bWritten = fwrite(*buf, 1, *bufSize, fStream);
//	fclose(fStream);
//	return 0;
//	//if (szFullFrame <= 0 || fFullFrame == nullptr)
//	//	return -1;
//	//
//	//*bufSize = szFullFrame;
//	//return 0;
//}
void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) {
	DummySink* sink = (DummySink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

// If you don't want to see debugging output for each received frame, then comment out the following line:
#define DEBUG_PRINT_EACH_RECEIVED_FRAME 1

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) {
	// We've just received a frame of data.  (Optionally) print out information about it:
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
	if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
	envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes" << " SSRC " << fSubsession.rtpSource()->SSRC();
	if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated) " ;
	char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the presentation time
	sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
	envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
	if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
		envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
	}
#ifdef DEBUG_PRINT_NPT
	envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
	envir() << "\n";
#endif
	if (durationInMicroseconds == ((unsigned int) -1))
	{
		continuePlaying();
		return;
	}
	szReceiveBuffer = frameSize;
	
	int searchLimit = szReceiveBuffer < (100 - 4) ? szReceiveBuffer : 100;
	if ((fReceiveBuffer[0] & 0x1F) == 0x5)
	{
		bIntraEncountered = true;
	}
	if (!bIntraEncountered)
	{
		for (int i = 0; i < searchLimit; i++)
		{
			if (fReceiveBuffer[i] == 0 && fReceiveBuffer[i + 1] == 0 && fReceiveBuffer[i + 2] == 1 && (fReceiveBuffer[i + 3] & 0x1F) == 5)
			{
				bIntraEncountered = true;
				break;
			}
		}
	}
	
	
	// Then continue, to request the next frame of data:
	
	//H264VideoRTPSource* e = H264VideoRTPSource::createNew(m_env, )
	//FramedSource* d = FramedSource::createNew();// (fSubsession.rtpSource());
	bool bBegin = ((MultiFramedRTPSource*)fSubsession.rtpSource())->getfCurrentPacketBeginsFrame();
	bool bEnd = ((MultiFramedRTPSource*)fSubsession.rtpSource())->getfCurrentPacketEndsFrame();
//	auto c = H264VideoStreamFramer::createNew(m_env, fSubsession.rtpSource(), true);
//	c->
//	H264VideoStreamFramer* c = new H264VideoStreamFramer(m_env, fSubsession.rtpSource(), true, true);
	unsigned nfameSizeAccum = 0;
	int frameNum = ((fReceiveBuffer[1] & 0x1F) >> 1);
	if (frameNum == lastFrameNum)
	{
		continuePlaying();
		return;
	}
	lastFrameNum = frameNum;
	if (bEnd == true && bIntraEncountered)
	{
		const char psc[4] = { 0x00, 0x00, 0x00, 0x01 };
	
		unsigned int numSProp = 0;
		auto sProps = parseSPropParameterSets(fSubsession.fmtp_spropparametersets(), numSProp);
		for (int i = 0; i < numSProp; i++)
		{
			//memcpy_s(&fFullFrame[nfameSizeAccum], MAX_FRAME_SIZE - nfameSizeAccum, psc, 4);
			//memcpy(&fFullFrame[nfameSizeAccum], psc, 4);
			//nfameSizeAccum += 4;
			//memcpy_s(&fFullFrame[nfameSizeAccum], MAX_FRAME_SIZE - nfameSizeAccum, sProps[i].sPropBytes, sProps[i].sPropLength);
			memcpy(&fFullFrame[nfameSizeAccum], sProps[i].sPropBytes, sProps[i].sPropLength);
			nfameSizeAccum += sProps[i].sPropLength;
		}
		
		//memcpy_s(&fFullFrame[nfameSizeAccum], MAX_FRAME_SIZE - nfameSizeAccum, psc, 4);
		memcpy(&fFullFrame[nfameSizeAccum], psc, 4);
		nfameSizeAccum += 4;
		//memcpy_s(&fFullFrame[nfameSizeAccum], MAX_FRAME_SIZE - nfameSizeAccum, fReceiveBuffer, szReceiveBuffer);
		memcpy(&fFullFrame[nfameSizeAccum], fReceiveBuffer, szReceiveBuffer);
		nfameSizeAccum += szReceiveBuffer;
		szFullFrame = nfameSizeAccum;
		std::vector<unsigned char> frameBuf(szFullFrame);
		std::copy(std::begin(fFullFrame), std::begin(fFullFrame) + szFullFrame, frameBuf.begin());

		{
			std::lock_guard<std::mutex> locker(frameBuffersMutex);
			char sFrameBufferSize[16];
			sprintf(sFrameBufferSize, "%d", frameBuffers.size());
			envir() << "DummySink::afterGettingFrame frameBuffers size " << sFrameBufferSize << " frame Num " << ((fReceiveBuffer[1] & 0x1F) >> 1) << "\n";
			frameBuffers.push(std::make_shared<std::vector<unsigned char>>(std::move(frameBuf)));
		}
		
		//FILE* fStream;
		//fStream = fopen("C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\testIn.264", "ab");
		//fwrite(fFullFrame, 1, szFullFrame, fStream);
		//fclose(fStream);
	}
	continuePlaying();

}

Boolean DummySink::continuePlaying() {
	if (fSource == NULL) return False; // sanity check (should not happen)

									   // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
		afterGettingFrame, this,
		onSourceClosure, this);

	return True;
}