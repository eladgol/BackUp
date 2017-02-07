#include "AndroidCalcLibrary.h"

#define PLATFORM_ANDROID 0
#define PLATFORM_IOS 1

char * AndroidCalcLibrary::getTemplateInfo()
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

AndroidCalcLibrary::AndroidCalcLibrary()
{
}

AndroidCalcLibrary::~AndroidCalcLibrary()
{
}
