#pragma once
typedef long long OCVCHandle;
class AndroidOpenCVComputeDLL {
public:
    static char * getTemplateInfo();
    AndroidOpenCVComputeDLL();
	~AndroidOpenCVComputeDLL();
	

	OCVCHandle CreateOCVCInstance();
	int DecompressImage(OCVCHandle handle, unsigned char* inBuf, unsigned long inSize, unsigned char* outBuf, unsigned long outAllocatedSize, unsigned long* outRealSize,
		unsigned long* compressionType, unsigned long* subCompressionType, unsigned long* width, unsigned long* height, unsigned long* bitDepth);

};
