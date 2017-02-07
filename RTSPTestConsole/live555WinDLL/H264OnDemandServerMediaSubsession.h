
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>
class H264OnDemandServerMediaSubsession : public OnDemandServerMediaSubsession {
public:
	static H264OnDemandServerMediaSubsession*
		createNew(UsageEnvironment& env, Boolean reuseFirstSource);
	void afterPlayingDummy1();
	void checkForAuxSDPLine1();
	DeviceSource* m_pDevSource = nullptr;
	void InjectFrame(unsigned char* buf, unsigned int bufSize);
protected:
	H264OnDemandServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
	// called only by createNew();
	virtual ~H264OnDemandServerMediaSubsession();
	

	void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
	virtual char const* getAuxSDPLine(RTPSink* rtpSink,
		FramedSource* inputSource);

	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
		unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
		unsigned char rtpPayloadTypeIfDynamic,
		FramedSource* inputSource);

private:
	char* fAuxSDPLine = nullptr;
	char fDoneFlag; // used when setting up "fAuxSDPLine"
	RTPSink* fDummyRTPSink = nullptr; // ditto
	
	std::mutex m;
	std::condition_variable cvNodata;
	std::atomic<bool> bDataPresent;// = false;
	H264or5VideoStreamFramer* videoSource = nullptr;
	std::atomic<bool> bDeviceFinished;
	std::vector<unsigned char> sps;
	std::vector<unsigned char> pps;
	void CopyHeaders(unsigned char* buf, unsigned int szBuf, int offsetPSCs, int lastOffset, int lastNALType);
	bool FillHeaders(unsigned char*& buf, unsigned int& szBuf, int& offsetPSCs);
};
