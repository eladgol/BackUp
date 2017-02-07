#include "DeviceSource.hh"
#include <GroupsockHelper.hh> // for "gettimeofday()"
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string.h>


//static uint8_t *buf = (uint8_t*)malloc(102400);
//static unsigned char buf[8192];
//int upp_stream;
//static uint8_t *bufPtr = buf;

DeviceSource* DeviceSource::createNew(UsageEnvironment& env, DeviceParameters params)
{

	return new DeviceSource(env, params);
}

EventTriggerId DeviceSource::eventTriggerId = 0;

unsigned DeviceSource::referenceCount = 0;

DeviceSource::DeviceSource(UsageEnvironment& env, DeviceParameters devParams) :FramedSource(env)
{

	if (referenceCount == 0)
	{

		//upp_stream = open("/dev/upp", O_RDWR);

	}
	++referenceCount;
	envir() << "DeviceSource::DeviceSource, reference count " << referenceCount << "\n\n";
	if (eventTriggerId == 0)
	{
		eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
	}

}

DeviceSource::~DeviceSource(void) {
	finishedCallback();
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [&] {return !bInjecting; });
		--referenceCount;
		envir() << "DeviceSource::~DeviceSource, reference count " << referenceCount;
		envir().taskScheduler().deleteEventTrigger(eventTriggerId);
		eventTriggerId = 0;

		

	if (referenceCount == 0)
	{

	}
	
}

int loop_count;

void DeviceSource::doGetNextFrame()
{

  // This function is called (by our 'downstream' object) when it asks for new data.

  // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
  if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
    handleClosure();
    return;
  }
	deliverFrame();

}

void DeviceSource::deliverFrame0(void* clientData)
{
	((DeviceSource*)clientData)->deliverFrame();
}

void DeviceSource::deliverFrame()
{
  // This function is called when new frame data is available from the device.
  // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
  //         by calling "gettimeofday()".
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
  //         to set this variable, because - in this case - data will never arrive 'early'.
  // Note the code below.

	if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet
	if (m_buf == 0)
	{
		std::unique_lock<std::mutex> lk(mBufInit);
		cvBufInit.wait_for(lk, std::chrono::seconds(1), [&]() { return m_buf != 0; });
		if (m_buf == 0)
			return;
	}
		
	u_int8_t* newFrameDataStart = (u_int8_t*)m_buf;             //(u_int8_t*) buf; //%%% TO BE WRITTEN %%%
	unsigned newFrameSize = m_bufSize; //%%% TO BE WRITTEN %%%

										 // Deliver the data here:
	if (newFrameSize > fMaxSize) {
		fFrameSize = fMaxSize;
		fNumTruncatedBytes = newFrameSize - fMaxSize;
	}
	else {
		fFrameSize = newFrameSize;
	}
	gettimeofday(&fPresentationTime, NULL);
	memmove(fTo, newFrameDataStart, fFrameSize);
	FramedSource::afterGetting(this);
}


// The following code would be called to signal that a new frame of data has become available.
// This (unlike other "LIVE555 Streaming Media" library code) may be called from a separate thread.
// (Note, however, that "triggerEvent()" cannot be called with the same 'event trigger id' from different threads.
// Also, if you want to have multiple device threads, each one using a different 'event trigger id', then you will need
// to make "eventTriggerId" a non-static member variable of "DeviceSource".)
void signalNewFrameData() {
  TaskScheduler* ourScheduler = NULL; //%%% TO BE WRITTEN %%%
  DeviceSource* ourDevice  = NULL; //%%% TO BE WRITTEN %%%

  if (ourScheduler != NULL) { // sanity check
    ourScheduler->triggerEvent(DeviceSource::eventTriggerId, ourDevice);
  }
}
void DeviceSource::InjectFrame(unsigned char* buf, unsigned int bufSize)
{
	// should change to buffer pool so no fragmentation
	std::unique_lock<std::mutex> lk(mBufInit);
	if (m_buf == 0)
	{
		m_bufSize = bufSize;
		m_bufAllocatedSize = m_bufSize * 2;
		m_buf = new unsigned char[m_bufAllocatedSize];
		envir() << "InjectFrame allocated buf to " << m_bufAllocatedSize << "\n";
	}
	else if (bufSize > m_bufAllocatedSize)
	{
		delete[] m_buf;
		m_bufSize = bufSize;
		m_bufAllocatedSize = m_bufSize * 2;
		m_buf = new unsigned char[m_bufAllocatedSize];
		envir() << "InjectFrame allocated buf to " << m_bufAllocatedSize << "\n";
	}
	memcpy(m_buf, buf, bufSize);
	cvBufInit.notify_all();
}