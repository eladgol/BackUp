/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2015 Live Networks, Inc.  All rights reserved.
// A template for a MediaSource encapsulating an audio/video input device
// 
// NOTE: Sections of this code labeled "%%% TO BE WRITTEN %%%" are incomplete, and needto be written by the programmer
// (depending on the features of the particulardevice).
// C++ header

#ifndef _DEVICE_SOURCE_HH
#define _DEVICE_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif

// The following class can be used to define specific encoder parameters
class DeviceParameters {
  //%%% TO BE WRITTEN %%%
};

class DeviceSource: public FramedSource {
public:
  static DeviceSource* createNew(UsageEnvironment& env,
				 DeviceParameters params);

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.
  void InjectFrame(unsigned char* buf, unsigned int bufSize)
  { 
	  // should change to buffer pool so no fragmentation
	  if (m_buf == nullptr)
	  {
		  m_bufSize = bufSize;
		  m_bufAllocatedSize = m_bufSize * 2;
		  m_buf = new unsigned char[m_bufAllocatedSize];
	  }
	  else if (bufSize > m_bufAllocatedSize)
	  {
		  delete[] m_buf;
		  m_bufSize = bufSize;
		  m_bufAllocatedSize = m_bufSize * 2;
		  m_buf = new unsigned char[m_bufAllocatedSize];
	  }
	  memcpy_s(m_buf, m_bufAllocatedSize, buf, bufSize);
  }
  virtual ~DeviceSource();
protected:
  DeviceSource(UsageEnvironment& env, DeviceParameters params);
  // called only by createNew(), or by subclass constructors
  

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

private:
  static void deliverFrame0(void* clientData);
  void deliverFrame();

private:
  static unsigned referenceCount; // used to count how many instances of this class currently exist
  DeviceParameters fParams;
private: 
	unsigned char* m_buf = nullptr;
	unsigned int m_bufSize = 0;
	unsigned int m_bufAllocatedSize = 0;
};

#endif
