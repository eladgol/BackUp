#include "RunRTSPServer.h"
#include "testRTSPClient.h"
//
#include "live555WinDLL.h"
#include "RTSPClientClasses.h"
#include "RunRTSPServer.h"
#include <map>
#include <memory>

std::map<int, std::shared_ptr<RunRTSPServer>> streamers;
std::map<int, std::shared_ptr<testRTSPClient>> clients;
int iStreamerHandle = 0;
int iClientHandle = 0;

int RTSPServer_Create()
{
	auto pStreamer = std::make_shared<RunRTSPServer>();
	auto streamersElement = std::make_pair(iStreamerHandle, pStreamer);
	streamers.insert(streamersElement);
	
	iStreamerHandle++;
	return (iStreamerHandle - 1);
}
void RTSPServer_Play(int handle, int port, int httpPortNum)
{
	streamers[handle]->PlayMain(port, httpPortNum);
}

void RTSPServer_InjectFrame(int handle, unsigned char * buf, unsigned int bufSize)
{
	streamers[handle]->InjectFrame(buf, bufSize);
}

LIVE555WINDLL_API int RTSPClient_Create()
{

	auto pClient = std::make_shared<testRTSPClient>();
	auto clientElement = std::make_pair(iClientHandle, pClient);
	clients.insert(clientElement);

	iClientHandle++;
	return (iClientHandle - 1);
}

LIVE555WINDLL_API void RTSPClient_GetFrame(int handle, unsigned char * buf, unsigned int bufAlocatedSize, unsigned int & bufRealSize)
{
	clients[handle]->GetFrame(&buf, bufAlocatedSize, &bufRealSize, true);
}

LIVE555WINDLL_API void RTSPClient_Listen(int handle/*, const char* sRTSP*/, int httpPortNum)
{
	clients[handle]->mainListener(/*sRTSP, */httpPortNum);
}
