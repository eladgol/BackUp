// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIVE555WINDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIVE555WINDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ___ANDROID____
#include <jni.h>
#ifdef LIVE555WINDLL_EXPORTS
#define LIVE555WINDLL_API extern "C" JNIEXPORT
#else
#define LIVE555WINDLL_API extern "C" JNIIMPORT
#endif
#else
#ifdef LIVE555WINDLL_EXPORTS
#define LIVE555WINDLL_API extern "C" __declspec(dllexport)
#else
#define LIVE555WINDLL_API extern "C" __declspec(dllimport)
#endif
#endif
LIVE555WINDLL_API int RTSPServer_Create();
LIVE555WINDLL_API void RTSPServer_InjectFrame(int handle, unsigned char* buf, unsigned int bufSize);
LIVE555WINDLL_API void RTSPServer_Play(int handle, int port, int httpPortNum);

LIVE555WINDLL_API int	RTSPClient_Create	();
LIVE555WINDLL_API void	RTSPClient_GetFrame	(int handle, unsigned char* buf, unsigned int bufAlocatedSize, unsigned int& bufRealSize);
LIVE555WINDLL_API void	RTSPClient_Listen	(int handle/*, const char* sRTSP*/, int httpPortNum);