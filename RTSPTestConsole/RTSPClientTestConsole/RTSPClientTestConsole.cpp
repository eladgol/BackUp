// RTSPClientTestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\live555WinDLL\live555WinDLL.h"
#include <thread>
const int MAX_FRAME_SIZE = 100 * 1024 * 1204;
unsigned char buf[MAX_FRAME_SIZE];
float const fps = 300;

int main()
{

	auto handle = RTSPClient_Create();
	std::thread tRTSP([handle]()
	{
		RTSPClient_Listen(handle, /*"rtsp://192.168.1.28:554/h264ESVideoTest", */0);
	});
	std::thread tGetFrame([&]()
	{
		bool bContinue = true;
		while (bContinue)
		{
			unsigned int bufSize;
			RTSPClient_GetFrame(handle, buf, MAX_FRAME_SIZE, bufSize);
			float msecPerFrame = (1.0 / fps) * 1000;
			std::this_thread::sleep_for(std::chrono::milliseconds((int)msecPerFrame));

		}
	});
	tRTSP.join();
    return 0;
}

