#pragma once
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <stdio.h>

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <mutex>
#include <condition_variable>
#include <atomic>
class H264VideoStreamer
{
public:
	H264VideoStreamer()
	{
		bDataPresent = false;
	}
	int PlayMain(int port, int httpPortNum = 0U);
	void InjectFrame(unsigned char* buf, unsigned int bufSize);
	char* GetURL() { return m_url; };
private:
	void play();
	static void afterPlaying(void* clientData);
private:
	DeviceSource* m_pDevSource = nullptr;
	UsageEnvironment* env = nullptr;

	H264VideoStreamFramer* videoSource = nullptr;
	RTPSink* videoSink = nullptr;
	char* m_url = nullptr;
	std::mutex m;
	std::condition_variable cvNodata;
	std::atomic<bool> bDataPresent;// = false;
	std::string data;
	bool ready = false;
};
