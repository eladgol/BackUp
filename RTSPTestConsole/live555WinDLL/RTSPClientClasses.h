#pragma once
// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <mutex>

const int MAX_FRAME_SIZE = 10 * 1024 * 1204;
class StreamClientState {
public:
	StreamClientState();
	virtual ~StreamClientState();

public:
	MediaSubsessionIterator* iter;
	MediaSession* session;
	MediaSubsession* subsession;
	TaskToken streamTimerTask;
	double duration;
};

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:
class DummySink : public MediaSink {
public:
	typedef  std::shared_ptr<std::vector<unsigned char>> FrameBuf;
	static DummySink* createNew(UsageEnvironment& env,
		MediaSubsession& subsession, // identifies the kind of data that's being received
		std::function< FrameBuf() > getFrameCallback,
		char const* streamId = NULL); // identifies the stream itself (optional)
	FrameBuf GetFrame(bool bCopy = true);
	DummySink(UsageEnvironment& env, MediaSubsession& subsession, std::function< FrameBuf() > getFrameCallback, char const* streamId);
private:

	// called only by "createNew()"
	virtual ~DummySink();

	static void afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes,
		struct timeval presentationTime,
		unsigned durationInMicroseconds);
	void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	// redefined virtual functions:
	virtual Boolean continuePlaying();

private:
	u_int8_t* fReceiveBuffer;
	unsigned szReceiveBuffer = 0;
	unsigned char fFullFrame[MAX_FRAME_SIZE];
	unsigned szFullFrame = 0;
	MediaSubsession& fSubsession;
	UsageEnvironment& m_env;
	char* fStreamId;
	std::mutex frameBuffersMutex;
	std::queue<FrameBuf> frameBuffers;
	FrameBuf currFrameBuf;
	std::function< FrameBuf() > m_callback;
	int lastFrameNum = -1; // hack to get frame num from slice header to return when we get same frame (I would get it over and over again)
	bool bIntraEncountered = false;
};


class ourRTSPClient : public RTSPClient {
public:
	static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel = 0,
		char const* applicationName = NULL,
		portNumBits tunnelOverHTTPPortNum = 0);

protected:
	ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
	// called only by createNew();
	virtual ~ourRTSPClient();

public:
	DummySink* pSink = nullptr;
	StreamClientState scs;
};

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "FileSink", for outputting the received data into a file (as is done by the "openRTSP" application).
// In this example code, however, we define a simple 'dummy' sink that receives incoming data, but does nothing with it.

class RTSPClientSingleton
{
public:
	static RTSPClientSingleton& getInstance()
	{
		static RTSPClientSingleton    instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}
private:
	RTSPClientSingleton() {}                    // Constructor? (the {} brackets) are needed here.

							  // C++ 03
							  // ========
							  // Dont forget to declare these two. You want to make sure they
							  // are unacceptable otherwise you may accidentally get copies of
							  // your singleton appearing.
	RTSPClientSingleton(RTSPClientSingleton const&);              // Don't Implement
	void operator=(RTSPClientSingleton const&); // Don't implement
	
							  // C++ 11
							  // =======
							  // We can use the better technique of deleting the methods
							  // we don't want.
public:
//	RTSPClientSingleton(RTSPClientSingleton const&) = delete;
//	void operator=(RTSPClientSingleton const&) = delete;
	std::vector<ourRTSPClient*> myRTSPClient;
	// Note: Scott Meyers mentions in his Effective Modern
	//       C++ book, that deleted functions should generally
	//       be public as it results in better error messages
	//       due to the compilers behavior to check accessibility
	//       before deleted status
};