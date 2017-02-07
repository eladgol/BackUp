
#include "testH264VideoStreamer.h"





// forward
			 //-------------------------------------------------------------------------
			 //Entry Point -> Main FUNCTION  
			 //-------------------------------------------------------------------------

int H264VideoStreamer::PlayMain( int port, int httpPortNum) 
{
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	// Create 'groupsocks' for RTP and RTCP:
	struct in_addr destinationAddress;
	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
	// Note: This is a multicast address.  If you wish instead to stream
	// using unicast, then you should use the "testOnDemandRTSPServer"
	// test program - not this test program - as a model.

	const unsigned short rtpPortNum = 18888;
	const unsigned short rtcpPortNum = rtpPortNum + 1;
	const unsigned char ttl = 255;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly(); // we're a SSM source

									   // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 60000;
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = 1024; // in kbps; for RTCP b/w share
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0'; // just in case
	RTCPInstance* rtcp
		= RTCPInstance::createNew(*env, &rtcpGroupsock,
			estimatedSessionBandwidth, CNAME,
			videoSink, NULL /* we're a server */,
			True /* we're a SSM source */);
	// Note: This starts RTCP running automatically

	RTSPServer* rtspServer = RTSPServer::createNew(*env, port);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}
	ServerMediaSession* sms
		= ServerMediaSession::createNew(*env, "ipcamera", "UPP Buffer",
			"Session streamed by \"testH264VideoStreamer\"",
			True /*SSM*/);

	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);

	m_url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << m_url << "\"\n";
	if (httpPortNum > 0)
	{
		const Port httpPort(httpPortNum);
		if (rtspServer->setUpTunnelingOverHTTP(httpPort) || rtspServer->setUpTunnelingOverHTTP(httpPort) || rtspServer->setUpTunnelingOverHTTP(httpPort))
		{

		}
	}
	// Start the streaming:
	*env << "Beginning streaming...\n";
	play();

	
	env->taskScheduler().doEventLoop(); // does not return

	return 0; // only to prevent compiler warning
}
void H264VideoStreamer::afterPlaying(void* clientData)
{
	((H264VideoStreamer*)clientData)->play();
}
void H264VideoStreamer::InjectFrame(unsigned char* buf, unsigned int bufSize)
{
	if (m_pDevSource)
	{
		m_pDevSource->InjectFrame(buf, bufSize);
		bDataPresent = true;
		cvNodata.notify_one();
	}
}
//------------------------------------------------------------------------
//PLAY FUNCTION () 
//------------------------------------------------------------------------
void H264VideoStreamer::play()
{

	// Open the input file as with Device as the source:
	DeviceParameters devParams;
	m_pDevSource	= DeviceSource::createNew(*env, devParams);
	if (m_pDevSource == NULL)
	{

		*env << "Unable to read from\"" << "Buffer"
			<< "\" as a byte-stream source\n";
		exit(1);
	}
	std::unique_lock<std::mutex> l(m);
	cvNodata.wait(l, [this]() {return (bDataPresent == true); });

	FramedSource* videoES = m_pDevSource;

	// Create a framer for the Video Elementary Stream:
	videoSource = H264VideoStreamFramer::createNew(*env, videoES, False);

	// Finally, start playing:
	*env << "Beginning to read from UPP...\n";
	videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}