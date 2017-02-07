#include "AndroidOpenCVComputeDLL.h"

#define PLATFORM_ANDROID 0
#define PLATFORM_IOS 1

char * AndroidOpenCVComputeDLL::getTemplateInfo()
{
#if PLATFORM == PLATFORM_IOS
	static char info[] = "Platform for iOS";
#elif PLATFORM == PLATFORM_ANDROID
	static char info[] = "Platform for Android";
#else
	static char info[] = "Undefined platform";
#endif

	return info;
}

AndroidOpenCVComputeDLL::AndroidOpenCVComputeDLL()
{
	
}

OCVCHandle AndroidOpenCVComputeDLL::CreateOCVCInstance()
{
	OCVCHandle handle;
	return handle;
}

int AndroidOpenCVComputeDLL::DecompressImage(OCVCHandle handle, unsigned char * inBuf, unsigned long inSize, unsigned char * outBuf, unsigned long outAllocatedSize, unsigned long * outRealSize, unsigned long * compressionType, unsigned long * subCompressionType, unsigned long * width, unsigned long * height, unsigned long * bitDepth)
{
	return 0;
}

AndroidOpenCVComputeDLL::~AndroidOpenCVComputeDLL()
{
}
