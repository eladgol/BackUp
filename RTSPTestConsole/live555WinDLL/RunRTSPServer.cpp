#include "RunRTSPServer.h"
#include "SSDP.h"

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
	char const* streamName, char** url)
{
	*url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream \n";
	env << "Play this stream using the URL \"" << *url << "\"\n";
}


RunRTSPServer::RunRTSPServer()
{
}

RunRTSPServer::~RunRTSPServer()
{
}

int RunRTSPServer::PlayMain(int port, int httpPortNum)
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	
	env = BasicUsageEnvironment::createNew(*scheduler);
	*env << "RunRTSPServer::PlayMain port " << port << " httpPortNum " << httpPortNum << "\n";

	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
													 // Repeat the above with each <username>, <password> that you wish to allow
													 // access to the server.
#endif
	int detailsPort = 49156;
	ssdp = new SSDPServer(port, detailsPort);


													 // Create the RTSP server:
	rtspServer = RTSPServer::createNew(*env, port, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	char const* descriptionString
		= "Session streamed by \"EladsRTSPServer\"";

	// Set up each of the possible streams that can be served by the
	// RTSP server.  Each such stream is implemented using a
	// "ServerMediaSession" object, plus one or more
	// "ServerMediaSubsession" objects for each audio/video substream.


	
	ServerMediaSession* sms
		= ServerMediaSession::createNew(*env, streamName, streamName,
			descriptionString);
	pH264SubSession = H264OnDemandServerMediaSubsession
		::createNew(*env, reuseFirstSource);
	sms->addSubsession((ServerMediaSubsession* )pH264SubSession);
	rtspServer->addServerMediaSession(sms);
	announceStream(rtspServer, sms, streamName, &m_url);

	// Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
	// Try first with the default HTTP port (80), and then with the alternative HTTP
	// port numbers (8000 and 8080).
	if (httpPortNum > 0)
	{
		//if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
		if (rtspServer->setUpTunnelingOverHTTP(httpPortNum))
		{
			*env << "\n(We use port " << httpPortNum << " ___ " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
		}
		else {
			*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
		}
		
	}
	
	env->taskScheduler().doEventLoop(); // does not return

	return 0; // only to prevent compiler warning
}

void RunRTSPServer::InjectFrame(unsigned char * buf, unsigned int bufSize)
{
	if (pH264SubSession)
		pH264SubSession->InjectFrame(buf, bufSize);
	
}
