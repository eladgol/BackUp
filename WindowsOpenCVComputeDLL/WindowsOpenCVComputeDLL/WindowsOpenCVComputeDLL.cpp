#include <C:\Code\opencv\sources\include\opencv2\opencv.hpp>
#include "WindowsOpenCVComputeDLL.h"
#include <tchar.h>
#include <stdio.h>
#include <vector>
#include "PostProcessingCallbacks.h"
using namespace cv;

inline int bitDepthToType(int bitDepth)
{
	return ((bitDepth == 32) ? CV_8UC4 : ((bitDepth == 24) ? CV_8UC3 :
		((bitDepth == 16) ? CV_8UC2 : CV_8UC1)));
}


inline int typeToBitDepth(int type)
{
	return ((type == CV_8UC4) ? 32 : ((type == CV_8UC3) ? 24 :
		((type == CV_8UC2) ? 16 : 8)));
}
inline int sizeOfMat(Mat mat)
{
	return (typeToBitDepth(mat.type()) / 8 * mat.size[0] * mat.size[1]);
}

OCVCHandle CreateOCVCInstance()
{
	std::shared_ptr<CWindowsOpenCVComputeDLL> pCWindowsOpenCVComputeDLL;
	CWindowsOpenCVComputeDLLFactory::Instance()->CreateNew(pCWindowsOpenCVComputeDLL);
	return (OCVCHandle(pCWindowsOpenCVComputeDLL.get()));
}
LIBRARY_API int DecompressImage(OCVCHandle handle, unsigned char * inBuf, unsigned long inSize, unsigned char * outBuf, unsigned long  outAllocatedSize, unsigned long * outRealSize, unsigned long * compressionType, unsigned long * subCompressionType, unsigned long * width, unsigned long * height, unsigned long * bitDepth)
{
	auto pAPI = CWindowsOpenCVComputeDLLFactory::Instance()->GetAPI(handle);
	if (pAPI == nullptr)
		return -1;
	return (pAPI->DecompressImage(inBuf, inSize, outBuf, outAllocatedSize, outRealSize, compressionType, subCompressionType, width, height, bitDepth));
}

/** @function thresh_callback */
int CWindowsOpenCVComputeDLL::CContour::CannyEdge(unsigned char* inBuf, unsigned long inSize, unsigned long width, unsigned long height, unsigned int bitDepth,
	unsigned char* outBuf, unsigned long outSize)
{
	src = Mat(height, width, bitDepthToType(bitDepth));
	*this = src;
	return (CannyEdge(outBuf, outSize));
}
int CWindowsOpenCVComputeDLL::CContour::CannyEdge(unsigned char * outBuf, unsigned long outSize)
{
	if (src.empty())
		return -1;
	Mat edges = Mat(src_gray.size[1], src_gray.size[0], src_gray.type(), outBuf);
	cv::Canny(src_gray, edges, thresh, thresh * 2, 3);
	cv::namedWindow("Edges", CV_WINDOW_AUTOSIZE);
	cv::imshow("Edges", edges);
	return 0;
}
void CWindowsOpenCVComputeDLL::CContour::CreateDisplay(std::string sName, cv::Mat image)
{
	cv::namedWindow(sName.c_str(), CV_WINDOW_AUTOSIZE);
	cv::imshow(sName.c_str(), image);
}
std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> CWindowsOpenCVComputeDLL::CContour::AddImage(int iProcessing, std::string sName, int width, int height, int depth, unsigned char* input, bool bCreateDisplay)
{
	ImageResultsList::iterator imageResultInputIter = m_imageResults.find(iProcessing);
	if (imageResultInputIter == m_imageResults.end())
	{
		if (depth == -1)
		{ // take depth from previous image
			ImageResultsList::iterator imageResultPreviousInputIter = m_imageResults.find(iProcessing - 1);
			if (imageResultPreviousInputIter == m_imageResults.end())
			{
				depth = typeToBitDepth(src_gray.type());
			}
			else
			{
				depth = typeToBitDepth(imageResultPreviousInputIter->second->pImage->type());
			}
		}
		
		std::shared_ptr<cv::Mat> pImage;
		if (input == nullptr)
		{
			pImage = std::make_shared<Mat>(Mat(height, width, bitDepthToType(depth)));
		}
		else
		{
			pImage = std::make_shared<Mat>(height, width, bitDepthToType(depth), input);

		}
		std::shared_ptr<ImageResult> pImgRes = std::make_shared<ImageResult>(sName, pImage);
		m_imageResults.insert(std::make_pair(iProcessing, pImgRes));
		if (bCreateDisplay)
		{
			CreateDisplay(sName, *pImage);
		}
		return pImgRes;
	}
	//ImageResult imgRes = imageResultInputIter->second;
	//std::shared_ptr<ImageResult> pImgRes(imgRes);
	return imageResultInputIter->second;;
}
std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::Knob> CWindowsOpenCVComputeDLL::CContour::AddKnob(int knobID, std::shared_ptr<KnobInternal> knobInternal, std::shared_ptr<KnobCallback> callback)
{
	std::shared_ptr<Knob> pKnob = std::make_shared<Knob>(callback, knobInternal);
	m_knobs.insert(std::make_pair(knobID, pKnob));
	ImageResultsList::iterator imageResultInputIter = m_imageResults.find(knobInternal->parentProcessorID);
	if (imageResultInputIter == m_imageResults.end())
	{
		std::string sErr = "CContour::AddKnob no such processing id " + knobInternal->parentProcessorID;
		sErr += "\n";
		//OutputDebugString(sErr.c_str());
		return nullptr;
	}
	else
	{
		CallbackIdentity* I = new std::pair<int, CContour*>(knobID, this);
		createTrackbar(knobInternal->sDisplayName, imageResultInputIter->second->sProcessingName, &knobInternal->iValue, knobInternal->maxValue, thresh_callback, I );
		thresh_callback(knobInternal->iValue, I);
	}
	
	return pKnob;
}
void CWindowsOpenCVComputeDLL::CContour::AddImageAndKnobToChain(std::string sImage, int processorID, std::shared_ptr<KnobInternalMap> knobs, std::shared_ptr<KnobCallback> knobCallback, int newBitDepth)
{
	std::shared_ptr<ImageResult> pOutImage = AddImage(processorID, sImage, src_gray.size[1], src_gray.size[0], newBitDepth);
	//knobInternalAdaptiveThreshold
	knobs->insert(std::make_pair("Process", std::make_shared<KnobInternal>("Process", processorID, 0, 1, 1)));
	for each (auto var in *knobs)
	{
		auto knob = var.second;
		std::shared_ptr<Knob> pKnob = AddKnob(m_iKnobID++, knob, knobCallback);
	}
	
}
//		cv::morphologyEx(inverse_output, fill_output, cv::MORPH_OPEN, kernel);
void CWindowsOpenCVComputeDLL::CContour::InitPostProcChain()
{
	int iProcessing = 0;
	std::shared_ptr<ImageResult> pInImage = AddImage(iProcessing++, "Source", src_gray.size[1], src_gray.size[0], typeToBitDepth(src_gray.type()), src_gray.data);

	
	//std::shared_ptr<KnobInternalMap> pKnobsEdge = std::make_shared<KnobInternalMap>();
	//std::shared_ptr<KnobCallback> pEdgeCallback = std::make_shared<KnobCallback>(EdgeCallback);
	//std::shared_ptr<KnobInternal> knobInternalEdge = std::make_shared<KnobInternal>("threshold", iProcessing, 0, 255, 127);
	//pKnobsEdge->insert(std::make_pair(knobInternalEdge->sDisplayName, knobInternalEdge));
	//AddImageAndKnobToChain("Edge", iProcessing++, pKnobsEdge, pEdgeCallback);

	//std::shared_ptr<KnobInternalMap> pKnobsAdaptiveThreshold = std::make_shared<KnobInternalMap>();
	//std::shared_ptr<KnobCallback> pCallbackAdaptiveThreshold = std::make_shared<KnobCallback>(AdaptiveThresholdCallback);
	//std::shared_ptr<KnobInternal> knobInternalAdaptiveThreshold = std::make_shared<KnobInternal>("AdaptiveMethod", iProcessing, 0, 1, 1);
	//std::shared_ptr<KnobInternal> knobInternalThresholdType = std::make_shared<KnobInternal>("ThresholdType", iProcessing, 0, 1, 1);
	//std::shared_ptr<KnobInternal> knobInternalBlockSize = std::make_shared<KnobInternal>("BlockSize", iProcessing, 1, 128, 11);
	//std::shared_ptr<KnobInternal> knobInternalSubtractedConstant = std::make_shared<KnobInternal>("SubtractedConstant", iProcessing, 1, 128, 1);
	//pKnobsAdaptiveThreshold->insert(std::make_pair(knobInternalAdaptiveThreshold->sDisplayName, knobInternalAdaptiveThreshold));
	//pKnobsAdaptiveThreshold->insert(std::make_pair(knobInternalBlockSize->sDisplayName, knobInternalBlockSize));
	//pKnobsAdaptiveThreshold->insert(std::make_pair(knobInternalSubtractedConstant->sDisplayName, knobInternalSubtractedConstant));
	//pKnobsAdaptiveThreshold->insert(std::make_pair(knobInternalThresholdType->sDisplayName, knobInternalThresholdType));
	//AddImageAndKnobToChain("AdaptiveThreshold", iProcessing++, pKnobsAdaptiveThreshold, pCallbackAdaptiveThreshold);

	std::shared_ptr<KnobInternalMap> pKnobsThreshold = std::make_shared<KnobInternalMap>();
	std::shared_ptr<KnobCallback> pCallbackThreshold = std::make_shared<KnobCallback>(ThresholdCallback);
	AddImageAndKnobToChain("Threshold", iProcessing++, pKnobsThreshold, pCallbackThreshold);


	//std::shared_ptr<KnobInternalMap> pKnobsMorph = std::make_shared<KnobInternalMap>();
	//std::shared_ptr<KnobCallback> pCallbackMorph = std::make_shared<KnobCallback>(MorphOpenCallback);
	//std::shared_ptr<KnobInternal> knobInternalMorphType = std::make_shared<KnobInternal>("MorphType", 2, 0, 7, 3);
	//std::shared_ptr<KnobInternal> knobInternalMorphSizeX = std::make_shared<KnobInternal>("MorphSizeX", 2, 3, 100, 7);
	//std::shared_ptr<KnobInternal> knobInternalMorphSizeY = std::make_shared<KnobInternal>("MorphSizeY", 2, 3, 100, 7);
	//pKnobsMorph->insert(std::make_pair(knobInternalMorphType->sDisplayName, knobInternalMorphType));
	//pKnobsMorph->insert(std::make_pair(knobInternalMorphSizeX->sDisplayName, knobInternalMorphSizeX));
	//pKnobsMorph->insert(std::make_pair(knobInternalMorphSizeY->sDisplayName, knobInternalMorphSizeY));
	//AddImageAndKnobToChain("Morph", iProcessing++, pKnobsMorph, pCallbackMorph);

	//std::shared_ptr<KnobInternalMap> pKnobsResize = std::make_shared<KnobInternalMap>();
	//std::shared_ptr<KnobCallback> pCallbackResize = std::make_shared<KnobCallback>(ResizeCallback);
	//std::shared_ptr<KnobInternal> knobInternalResizeX = std::make_shared<KnobInternal>("X", iProcessing, 1, 16, src_gray.cols);
	//std::shared_ptr<KnobInternal> knobInternalResizeInterpolation = std::make_shared<KnobInternal>("Interpolation", iProcessing, 0, 4, 1);
	//pKnobsResize->insert(std::make_pair(knobInternalResizeX->sDisplayName, knobInternalResizeX));
	//pKnobsResize->insert(std::make_pair(knobInternalResizeInterpolation->sDisplayName, knobInternalResizeInterpolation));
	//AddImageAndKnobToChain("Resize", iProcessing++, pKnobsResize, pCallbackResize);

	std::shared_ptr<KnobInternalMap> pKnobsContour = std::make_shared<KnobInternalMap>();
	std::shared_ptr<KnobCallback> pCallbackContour = std::make_shared<KnobCallback>(CounterCallback);
	std::shared_ptr<KnobInternal> knobInternalContourMode = std::make_shared<KnobInternal>("ContourMode", iProcessing, 0, 4, 3);
	std::shared_ptr<KnobInternal> knobInternalContourAprox = std::make_shared<KnobInternal>("ContourAprox", iProcessing, 0, 5, 3);	
	pKnobsContour->insert(std::make_pair(knobInternalContourMode->sDisplayName, knobInternalContourMode));
	pKnobsContour->insert(std::make_pair(knobInternalContourAprox->sDisplayName, knobInternalContourAprox));
	AddImageAndKnobToChain("Contour", iProcessing++, pKnobsContour, pCallbackContour, 24);
	//std::shared_ptr<ImageResult> pOutImage = AddImage(1, "Edge", src_gray.size[1], src_gray.size[0], typeToBitDepth(src_gray.type()));
	//std::shared_ptr<KnobInternal> knobInternalEdge = std::make_shared<KnobInternal>();
	//knobInternalEdge->sDisplayName = "threshold";
	//knobInternalEdge->minValue = 0;
	//knobInternalEdge->maxValue = 255;
	//knobInternalEdge->iValue = (knobInternalEdge->maxValue - knobInternalEdge->minValue) / 2;
	//knobInternalEdge->parentProcessorID = 1;
	//std::shared_ptr<KnobCallback> callback = std::make_shared<KnobCallback>(EdgeCallback);// (EdgeCallback);// = std::make_shared<KnobCallback>();
	//std::shared_ptr<Knob> knob = AddKnob(0, knobInternalEdge, callback);


}
void CWindowsOpenCVComputeDLL::CContour::PresentResults()
{
	for each (auto result in m_imageResults)
	{
		std::string name = result.second->sProcessingName;
		cv::Mat image = *(result.second->pImage);
		cv::namedWindow(name.c_str(), CV_WINDOW_AUTOSIZE);
		cv::imshow(name.c_str(), image);
	}
}
void CWindowsOpenCVComputeDLL::CContour::thresh_callback(int value, void* abstractContext)
{
	CallbackIdentity* I = (CallbackIdentity*)abstractContext;
	I->second->PerformKnobCallback(I->first);
	I->second->PresentResults();
}
int CWindowsOpenCVComputeDLL::CContour::PerformKnobCallback(int knobID)
{
	Knobs::iterator currKnobIter = m_knobs.find(knobID);
	if (currKnobIter == m_knobs.end())
	{
		std::string sErr = "CContour::PerformCallback no such knobID " + knobID;
		sErr += "\n";
		//OutputDebugString(sErr.c_str());
		return -1;
	}
	std::shared_ptr<Knob> pCurrKnob(currKnobIter->second);

	ImageResultsList::iterator imageResultInputIter = m_imageResults.find(pCurrKnob->knobInternal->parentProcessorID  - 1);
	if (imageResultInputIter == m_imageResults.end())
	{ 
		std::string sErr = "CContour::PerformCallback no such imageResult " + pCurrKnob->knobInternal->parentProcessorID;
		sErr += "\n";
		//OutputDebugString(sErr.c_str());
		return -1;
	}
	std::shared_ptr<ImageResult> imageResultInput = m_imageResults[pCurrKnob->knobInternal->parentProcessorID - 1];

	ImageResultsList::iterator imageResultOutputIter = m_imageResults.find(pCurrKnob->knobInternal->parentProcessorID);
	if (imageResultOutputIter == m_imageResults.end())
	{
		std::string sErr = "CContour::PerformCallback no such imageResult " + pCurrKnob->knobInternal->parentProcessorID;
		sErr += "\n";
		//OutputDebugString(sErr.c_str());
		return -1;
	}
	std::shared_ptr<ImageResult> imageResultOutput = m_imageResults[pCurrKnob->knobInternal->parentProcessorID];
	std::shared_ptr<KnobInternalMap> sameKnobsMap = AllInternalKnobsOfSameProcessing(pCurrKnob->knobInternal->parentProcessorID);
	try
	{
		return PerformCallback(imageResultInput, imageResultOutput, sameKnobsMap, pCurrKnob->callback);
	}
	catch (std::exception e)
	{
		std::string sException = e.what();
		std::string sErr = "CContour::PerformCallback Exception " + sException;
		sErr += "\n";
		//OutputDebugString(sErr.c_str());
		return -1;
	}
}
//
//	std::vector<std::vector<Point> > contours;
//	std::vector<Vec4i> hierarchy;
//	
//	Mat canny_output = Mat(src_gray.size[1], src_gray.size[0], .type());
//	Mat inverse_output = Mat(context->src_gray.size[1], context->src_gray.size[0], context->src_gray.type());
//	Mat fill_output = Mat(context->src_gray.size[1], context->src_gray.size[0], context->src_gray.type());
//	context->CannyEdge(canny_output.data, sizeOfMat(canny_output));
//	bitwise_not(canny_output, inverse_output);
//	namedWindow("inverse_output", CV_WINDOW_AUTOSIZE);
//	imshow("inverse_output", inverse_output);
//	/// Find contours
//	try
//	{
//		
//		auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7,7));
//		cv::morphologyEx(inverse_output, fill_output, cv::MORPH_OPEN, kernel);
//		namedWindow("filled", CV_WINDOW_AUTOSIZE);
//		imshow("filled", fill_output);
//		//findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//	}
//	catch (std::exception e)
//	{		
//		auto x = e.what();
//	}
//	try
//	{
//		
//		findContours(canny_output, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//
//		//findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//	}
//	catch (std::exception e)
//	{
//		auto x = e.what();
//	}
//	RNG rng;
//	/// Draw contours
//	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
//	for (int i = 0; i< contours.size(); i++)
//	{
//		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//		//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
//		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
//	}
//	
//	/// Show in a window
//	namedWindow("drawing", CV_WINDOW_AUTOSIZE);
//	imshow("drawing", drawing);
//	Mat mask = Mat::zeros(canny_output.rows, canny_output.cols, CV_8UC1);
//	drawContours(mask, contours, -1, Scalar(255), CV_FILLED);
//	namedWindow("masked", CV_WINDOW_AUTOSIZE);
//	imshow("masked", mask);
//}

int CWindowsOpenCVComputeDLL::CContour::Contour(unsigned char* outBuf, unsigned long outSize)
{
	if (src.empty())
		return -1;
	

	/// Create Window
	char* source_window = "Source";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, src);

	createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback, this);
	thresh_callback(0, this);

	waitKey(0);
	return(0);
}
int CWindowsOpenCVComputeDLL::CContour::Contour(unsigned char* inBuf, unsigned long inSize, unsigned long width, unsigned long height, unsigned int bitDepth,
	unsigned char* outBuf, unsigned long outSize)
{
	src = Mat(height, width, bitDepthToType(bitDepth));
	*this = src;
	return Contour(outBuf, outSize);
}



int CWindowsOpenCVComputeDLL::DecompressImage(unsigned char* inBuf, unsigned long inSize, unsigned char* outBuf, unsigned long outAllocatedSize, unsigned long* outRealSize,
	unsigned long* compressionType, unsigned long* subCompressionType, unsigned long* width, unsigned long* height, unsigned long* bitDepth)
{
	
	Mat image = Mat(1, inSize, CV_8UC1, inBuf);
	auto imageData = cvDecodeImage(&CvMat(image));

	if (outAllocatedSize < imageData->imageSize)
	{
		return -1;
	}
	int widthBytes = imageData->width * (imageData->depth / 8) * imageData->nChannels;
	if (imageData->widthStep > widthBytes)
	{
		for (int y = 0; y < imageData->height; y++)
		{
			int outStartLine = y * widthBytes;
			int inStartLine = y * imageData->widthStep;
			int outAllocatedSizeLeft = outAllocatedSize - outStartLine;
			memcpy_s(&outBuf[outStartLine], outAllocatedSizeLeft, &imageData->imageData[inStartLine], widthBytes);
		}
		*outRealSize = imageData->height * widthBytes;
		
	}
	else
	{
		memcpy_s(outBuf, outAllocatedSize, imageData->imageData, imageData->imageSize);
		*outRealSize = imageData->imageSize;
	}
	
	*compressionType = *((unsigned long*)imageData->colorModel);
	*bitDepth = imageData->depth * imageData->nChannels;
	*subCompressionType = 0;
	*width = imageData->width;
	*height = imageData->height;
	Mat image1 = Mat(imageData->height, imageData->width, bitDepthToType(*bitDepth), outBuf);
	cvNamedWindow("Window", CV_WINDOW_AUTOSIZE);
	imshow("Window", image1);
	Mat contourImage = image1;
	countour = image1;
	//countour.Contour(contourImage.data, sizeOfMat(contourImage));
	countour.InitPostProcChain();
	return 0;
}
// This is the constructor of a class that has been exported.
// see WindowsOpenCVComputeDLL.h for the class definition
CWindowsOpenCVComputeDLL::CWindowsOpenCVComputeDLL()
{
    return;
}

CWindowsOpenCVComputeDLL::~CWindowsOpenCVComputeDLL()
{
}


int CWindowsOpenCVComputeDLLFactory::CreateNew(std::shared_ptr<CWindowsOpenCVComputeDLL>& API)
{
	API = std::make_shared<CWindowsOpenCVComputeDLL>();
	components.insert(std::make_pair((OCVCHandle)API.get(), API));
	return 0;
}

std::shared_ptr<CWindowsOpenCVComputeDLL> CWindowsOpenCVComputeDLLFactory::GetAPI(OCVCHandle handle)
{
	auto component = components.find(handle);
	std::pair <OCVCHandle, std::shared_ptr<CWindowsOpenCVComputeDLL>> iter = std::make_pair(0, nullptr);
	
	if (component != components.end())
	{
		iter = *component;
	}
	
	auto component2 = components[handle];
	return (iter.second);
}

int CWindowsOpenCVComputeDLLFactory::Remove(std::shared_ptr<CWindowsOpenCVComputeDLL> API)
{
	components.erase((OCVCHandle)API.get());
	return 0;
}
