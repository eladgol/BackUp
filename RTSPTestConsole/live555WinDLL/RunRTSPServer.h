#pragma once
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "H264OnDemandServerMediaSubsession.h"
#include "SSDP.h"
class RunRTSPServer
{
public:
	RunRTSPServer();
	virtual ~RunRTSPServer();
	int PlayMain(int port, int httpPortNum);
	void InjectFrame(unsigned char* buf, unsigned int bufSize);
	char* GetURL() { return m_url; };
private:
	UsageEnvironment* env;
	Boolean reuseFirstSource = False;
	Boolean iFramesOnly = False;
	char* m_url = nullptr;
	RTSPServer* rtspServer;
	char const* streamName = "h264ESVideoTest";
	H264OnDemandServerMediaSubsession* pH264SubSession = nullptr;
	// ssdp
	SSDPServer* ssdp;
};

