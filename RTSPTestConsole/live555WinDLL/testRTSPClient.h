#pragma once
#include "RTSPClientClasses.h"
#include "SSDP.h"
#include <vector>
static char eventLoopWatchVariable = 0;
class testRTSPClient
{
private:
	
	// RTSP 'response handlers':
	static void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
	static void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
	static void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

	// Other event handler functions:
	static void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
	static void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
	static void streamTimerHandler(void* clientData);

	// Used to iterate through each stream's 'subsessions', setting up each one:
	static void setupNextSubsession(RTSPClient* rtspClient);

	// Used to shut down and close a stream (including its "RTSPClient" object):
	static void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);
	// called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")
public:
	TaskScheduler* scheduler;
	UsageEnvironment* env;
	ourRTSPClient* rtspClient;
	testRTSPClient()
	{
		scheduler = BasicTaskScheduler::createNew();
		env = BasicUsageEnvironment::createNew(*scheduler);
	}
	
	// The main streaming routine (for each "rtsp://" URL):
	void openURL(UsageEnvironment& env, char const* progName/*, char const* rtspURL*/, int httpPortNum = 0);
	int GetFrame(unsigned char** buf, const unsigned allocSize, unsigned* bufSize, bool bCopy = true);

public:
	int mainListener(/*const char* sURL, */int httpPortNum);
	SSDPClient ssdp;
};