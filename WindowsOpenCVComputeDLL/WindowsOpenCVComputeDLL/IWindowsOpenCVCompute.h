#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
#define EXTERN_DLL_IMPORT extern "C" __declspec(dllimport)

#ifdef LIBRARY_EXPORTS
#    define LIBRARY_API EXTERN_DLL_EXPORT
#else
#    define LIBRARY_API EXTERN_DLL_IMPORT
#endif

typedef long long OCVCHandle;

LIBRARY_API OCVCHandle CreateOCVCInstance();
LIBRARY_API int DecompressImage(OCVCHandle handle, unsigned char* inBuf, unsigned long inSize, unsigned char* outBuf, unsigned long outAllocatedSize, unsigned long* outRealSize,
	unsigned long* compressionType, unsigned long* subCompressionType, unsigned long* width, unsigned long* height, unsigned long* bitDepth);
