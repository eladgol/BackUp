#pragma once
#include "IWindowsOpenCVCompute.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>

class CWindowsOpenCVComputeDLL
{
public:
	CWindowsOpenCVComputeDLL();
	int DecompressImage(unsigned char* inBuf, unsigned long inSize, unsigned char* outBuf, unsigned long outAllocatedSize, unsigned long* outRealSize,
		unsigned long* compressionType, unsigned long* subCompressionType, unsigned long* width, unsigned long* height, unsigned long* bitDepth);
	~CWindowsOpenCVComputeDLL();

	class CContour
	{
	public :
		CContour &operator =(const cv::Mat& _src)
		{
			src = _src;
			/// Convert image to gray and blur it
			cvtColor(src, src_gray, CV_BGR2GRAY);
			//blur(src_gray, src_gray, cv::Size(3, 3));
			return *this;
		}
		int CannyEdge(unsigned char* inBuf, unsigned long inSize, unsigned long width, unsigned long height, unsigned int bitDepth,
			unsigned char* outBuf, unsigned long outSize);
		
		int Contour(unsigned char* inBuf, unsigned long inSize, unsigned long width, unsigned long height, unsigned int bitDepth,
			unsigned char* outBuf, unsigned long outSize);
		int Contour(unsigned char* outBuf, unsigned long outSize);
		int CannyEdge(unsigned char* outBuf, unsigned long outSize);
		void InitPostProcChain();
		class ImageResult
		{
		public:
			std::string sProcessingName;
			std::shared_ptr<cv::Mat> pImage;
			std::shared_ptr<cv::Mat> previousImageToBypassTo;
			ImageResult(std::string _sProcessingName, std::shared_ptr<cv::Mat> _pImage, std::shared_ptr<cv::Mat> _pOutImage = nullptr)
			{
				sProcessingName = _sProcessingName;
				pImage = _pImage;
				
			}
			ImageResult()
			{
				sProcessingName = "";
				pImage = nullptr;
				//previousImageToBypassTo = nullptr;
			}
			
		};
		class KnobInternal
		{
		public:
			std::string sDisplayName;
			int parentProcessorID;
			int iValue = 50;
			int maxValue = 100;
			int minValue = 0;
			std::string sValue = "";
			KnobInternal(std::string _sDisplayName, int _parentProcessorID,
				int _minValue, int _maxValue, int _iValue, std::string _sValue = "")
			{
				sDisplayName = _sDisplayName;
				parentProcessorID = _parentProcessorID;
				maxValue = _maxValue;
				minValue = _minValue;
				iValue = _iValue;// (maxValue - minValue) / 2;
				sValue = _sValue;
			}
			KnobInternal()
			{
				sDisplayName = "";
				parentProcessorID = 0;
				int iValue = 50;
				int maxValue = 100;
				int minValue = 0;
				std::string sValue = "";
			}
		};
		typedef std::map<std::string, std::shared_ptr<KnobInternal>> KnobInternalMap;
	private:
		static void thresh_callback(int value, void* context);
		cv::Mat src;
		cv::Mat src_gray;
		int thresh = 100;
		int max_thresh = 255;
		cv::RNG rng;
		
		typedef std::map<int, std::shared_ptr<ImageResult>> ImageResultsList;
		std::shared_ptr<ImageResult> AddImage(int iProcessing, std::string Name, int width, int height, int depth = -1, unsigned char* input = nullptr, bool bCreateDisplay = true);
		ImageResultsList m_imageResults;
		void PresentResults();

		typedef int(*pKnobCallback) (std::shared_ptr<ImageResult> inImage, std::shared_ptr<ImageResult> outImage, std::shared_ptr<KnobInternalMap> knobInternal);
		typedef std::function<int(std::shared_ptr<ImageResult> inImage, std::shared_ptr<ImageResult> outImage, std::shared_ptr<KnobInternalMap> knobInternal)> KnobCallback;
		struct Knob
		{
			std::shared_ptr<KnobCallback> callback;
			std::shared_ptr<KnobInternal> knobInternal;
			Knob(std::shared_ptr<KnobCallback> _callback, std::shared_ptr<KnobInternal> _knobInternal)
			{
				callback = _callback;
				knobInternal = _knobInternal;
			}
		};

		std::shared_ptr<Knob> AddKnob(int knobID, std::shared_ptr<KnobInternal> knobInternal, std::shared_ptr<KnobCallback> callback);
		typedef std::map<int, std::shared_ptr<Knob>> Knobs; 
		Knobs m_knobs;
		typedef std::pair<int, CContour*> CallbackIdentity;
		int PerformKnobCallback(int knobID);
		int PerformCallback(std::shared_ptr<ImageResult> inImage, std::shared_ptr<ImageResult> outImage, std::shared_ptr<KnobInternalMap> knobInternals, std::shared_ptr<KnobCallback> callback)
		{
			return (*callback)(inImage, outImage, knobInternals);
		}
		//static int EdgeCallback(ImageResult* inImage, ImageResult* outImage, KnobInternal* knobInternal);
		std::shared_ptr<KnobInternalMap> AllInternalKnobsOfSameProcessing(int iProcessing)
		{
			std::shared_ptr<KnobInternalMap> pKnobs = std::make_shared<KnobInternalMap>();
			for each (auto var in m_knobs)
			{
				std::shared_ptr<Knob> pKnob = var.second;
				if (pKnob->knobInternal->parentProcessorID == iProcessing)
				{
					pKnobs->insert(std::make_pair(pKnob->knobInternal->sDisplayName, pKnob->knobInternal));
				}
			}
			return pKnobs;
		}
		void AddImageAndKnobToChain(std::string sImage, int processorID, std::shared_ptr<KnobInternalMap> knobs, std::shared_ptr<KnobCallback> knobCallback, int bNewDepthType = -1);
		void CreateDisplay(std::string sName, cv::Mat image);
		long m_iKnobID = 0;
	};
	CContour countour;
};
class CWindowsOpenCVComputeDLLFactory {
public:
	static CWindowsOpenCVComputeDLLFactory* Instance()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_instance == 0) {
			_instance = new CWindowsOpenCVComputeDLLFactory;
		}
		return _instance;
	}
	int CreateNew(std::shared_ptr<CWindowsOpenCVComputeDLL>& API);
	std::shared_ptr<CWindowsOpenCVComputeDLL> GetAPI(OCVCHandle handle);
	int Remove(std::shared_ptr<CWindowsOpenCVComputeDLL> API);
protected:
	CWindowsOpenCVComputeDLLFactory()
	{
		_instance = 0;
	};
private:
	static CWindowsOpenCVComputeDLLFactory* _instance;
	static std::mutex _mutex;
	std::unordered_map<OCVCHandle, std::shared_ptr<CWindowsOpenCVComputeDLL>> components;
};
std::mutex CWindowsOpenCVComputeDLLFactory::_mutex;
CWindowsOpenCVComputeDLLFactory* CWindowsOpenCVComputeDLLFactory::_instance;