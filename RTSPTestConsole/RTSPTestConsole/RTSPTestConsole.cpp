// RTSPTestConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <functional>
#include "..\live555WinDLL\live555WinDLL.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


float const fps = 30;


std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
int main()
{
	//HANDLE hPipe;



	//hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\PipeRTSPTest"),
	//	PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
	//	PIPE_WAIT,
	//	1,
	//	1024 * 16,
	//	1024 * 16,
	//	NMPWAIT_USE_DEFAULT_WAIT,
	//	NULL);

	
	
	FILE* stream;
	auto sz = filesize("C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\OneIFrame.264.bin");
	unsigned char* buf = new unsigned char[sz];
	auto err = fopen_s(&stream, "C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\OneIFrame.264.bin", "rb");
	auto szRead = fread_s(buf, sz, 1, sz, stream);
	fclose(stream);

	FILE* streamB;
	auto szB = filesize("C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\OneIFrame.264.bin");
	unsigned char* bufB = new unsigned char[szB];
	auto errB = fopen_s(&streamB, "C:\\Code\\MyCode\\RTSPTestConsole\\Debug\\OneIFrameB.264.bin", "rb");
	auto szReadB = fread_s(bufB, sz, 1, sz, stream);
	fclose(streamB);
	auto handle = RTSPServer_Create();
	int port = 8554;
	static std::atomic<bool> bConintueInjecting = true;
	static std::mutex lock;
	static std::condition_variable cv;
	RTSPServer_InjectFrame(handle, buf, sz);
	std::thread tFrameInject([&]()
	{
		bool bContinue = true;
		while (bConintueInjecting)
		{

			RTSPServer_InjectFrame(handle, buf, sz);
			float msecPerFrame = (1.0 / fps) * 1000;
			std::this_thread::sleep_for(std::chrono::milliseconds((int)msecPerFrame));
			
		}
		std::unique_lock<std::mutex> lk(lock);
		cv.wait(lk);
	});
	std::thread tRTSP([handle, port]()
	{
		RTSPServer_Play(handle, port, 0);
	});


	

	// ("C:\\Code\\RTSPTestConsole\\Debug\\test.264");
	tRTSP.join();
	bConintueInjecting = false;
	//std::lock_guard<std::mutex> lk(lock);
	//
	//cv.notify_all();
	tFrameInject.join();
	delete[] buf;
    return 0;
}


