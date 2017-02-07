#pragma once
#include <C:\Code\opencv\sources\include\opencv2\opencv.hpp>
#include "WindowsOpenCVComputeDLL.h"
#include <tchar.h>
#include <stdio.h>
#include <vector>
inline int checkResized(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, bool bUpdateResolution = true)
{
	if (inImage->previousImageToBypassTo.get() != nullptr)
	{
		inImage->pImage = inImage->previousImageToBypassTo;
	}
	if (inImage.get() == nullptr)
	{
		return -1;
	}
	//(inImage->pImage->cols != outImage->pImage->cols || inImage->pImage->rows != outImage->pImage->rows)
	if (bUpdateResolution)
	{
		outImage->pImage->create(inImage->pImage->rows, inImage->pImage->cols, outImage->pImage->type());
	}
	return 0;
}
int EdgeCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	auto inimage = *(inImage->pImage);
	auto outimage = *(outImage->pImage);

	auto pField = (*knobInternal)["threshold"];
	if (pField.get() == nullptr)
		return -1;
	auto val = pField->iValue;

	if (inImage->pImage->cols != outImage->pImage->cols || inImage->pImage->rows != outImage->pImage->rows)
	{
		outImage->pImage->create(inImage->pImage->rows, inImage->pImage->cols, outImage->pImage->type());
	}
	if (checkResized(inImage, outImage) == -1)
		return -1;
	cv::Canny(inimage, outimage, val, val * 2, 3);
	return 0;
};
int InverseCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	cv::bitwise_not(*(inImage->pImage), *(outImage->pImage));
	return 0;
};
int MorphOpenCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	auto pField = (*knobInternal)["MorphType"];
	if (pField.get() == nullptr)
		return -1;
	auto morphType = pField->iValue;
	if (morphType < 0 || morphType > 7)
		return -1;
	auto pField1 = (*knobInternal)["MorphSizeX"];
	if (pField1.get() == nullptr)
		return -1;
	auto morphSizeX = pField1->iValue;
	if (morphSizeX < 3 || morphSizeX > 100)
		return -1;
	auto pField2 = (*knobInternal)["MorphSizeY"];
	if (pField2.get() == nullptr)
		return -1;
	auto morphSizeY = pField2->iValue;
	if (morphSizeY < 3 || morphSizeY > 100)
		return -1;
	if (checkResized(inImage, outImage) == -1)
		return -1;
	auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morphSizeX, morphSizeY));
	cv::morphologyEx(*(inImage->pImage), *(outImage->pImage), (cv::MorphTypes)morphType, kernel);
	return 0;
};
int CounterCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	auto pField = (*knobInternal)["ContourMode"];
	if (pField.get() == nullptr)
		return -1;
	auto countourMode = pField->iValue;
	if (countourMode < 0 || countourMode > 4)
		return -1;
	auto pField1 = (*knobInternal)["ContourAprox"];
	if (pField1.get() == nullptr)
		return -1;
	auto countourAprox = pField1->iValue;
	if (countourAprox < 1 || countourAprox > 5) // 0 crashes
		return -1;
	if (checkResized(inImage, outImage) == -1)
		return -1;
	std::vector<std::vector<cv::Point> > contours;
	cv::Mat inImageColor;
	inImage->pImage->convertTo(inImageColor, CV_8U);
	//std::shared_ptr<cv::Mat> inImageColor = std::make_shared<cv::Mat>();// = cv::Mat(inImage->pImage->rows, inImage->pImage->cols, CV_8UC3);
	
	//cv::cvtColor(*inImage->pImage, inImageColor, CV_GRAY2RGB, 3);
	findContours(inImageColor, contours, countourMode, countourAprox);
	cv::RNG rng;
	for (int i = 0; i< contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(*(outImage->pImage), contours, i, color, CV_FILLED);
	}
	
	return 0;
}
int interpolationSliderToRealValue(int sliderVal)
{
	int val = 0;
	switch (sliderVal)
	{
	case 0:
		val = cv::INTER_LINEAR;
		break;
			/** bicubic interpolation */
	case 1:
		val = cv::INTER_CUBIC;
		break;
			/** resampling using pixel area
			it gives moire'-free results. B
			method. */
	case 2:
		val = cv::INTER_AREA;
		break;
			/** Lanczos interpolation over*/
	case 3:
		val = cv::INTER_LANCZOS4;
		break;
			/** mask for interpolation code
			INTER_MAX            = 7,
			/** flag, fills all of the dest
			source image, they are set to z
			WARP_FILL_OUTLIERS   = 8,
			/** flag, inverse transformatio

			For example, polar transforms:
			- flag is __not__ set: \f$dst(
			- flag is set: \f$dst(x,y) = sr
			*/
	case 4:
		val = cv::WARP_INVERSE_MAP;
		break;
		
	default:
		val = -1;
		break;
	};
	return val;
}
int ResizeCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	//auto pField = (*knobInternal)["Dstx"];
	//if (pField.get() == nullptr)
	//	return -1;
	//auto dstX = pField->iValue;
	//if (dstX < 16 || dstX > 4000)
	//	return -1;
	//auto pField1 = (*knobInternal)["Dsty"];
	//if (pField1.get() == nullptr)
	//	return -1;
	//auto dstY = pField1->iValue;
	//if (dstY < 16 || dstY > 4000)
	//	return -1;
	auto pField = (*knobInternal)["X"];
	if (pField.get() == nullptr)
		return -1;
	auto X = pField->iValue;
	if (X < 1 || X > 16)
		return -1;
	auto pField2 = (*knobInternal)["Interpolation"];
	if (pField2.get() == nullptr)
		return -1;
	auto interpolation = interpolationSliderToRealValue(pField2->iValue);
	if (interpolation == -1)
		return -1;
	if (checkResized(inImage, outImage, false) == -1)
		return -1;
	auto inimage = *(inImage->pImage);
	cv::Mat outImageTemp;

	cv::resize(inimage, outImageTemp, { inimage.cols * X, inimage.rows * X }, interpolation);
	(*outImage->pImage) = outImageTemp.clone();
	return 0;

}

int AdaptiveThresholdCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	auto inimage = *(inImage->pImage);
	auto outimage = *(outImage->pImage);

	auto pField = (*knobInternal)["AdaptiveMethod"];
	if (pField.get() == nullptr)
		return -1;
	auto adaptiveMethod = pField->iValue;
	if (adaptiveMethod < 0 || adaptiveMethod > 1)
		return -1;
	
	auto pField1 = (*knobInternal)["ThresholdType"];
	if (pField1.get() == nullptr)
		return -1;
	auto thresholdType = pField1->iValue;
	if (thresholdType < 0 || thresholdType > 1)
		return -1;
	if (inImage->pImage->cols != outImage->pImage->cols || inImage->pImage->rows != outImage->pImage->rows)
	{
		outImage->pImage->create(inImage->pImage->rows, inImage->pImage->cols, outImage->pImage->type());
	}
	auto pField2 = (*knobInternal)["BlockSize"];
	if (pField2.get() == nullptr)
		return -1;
	auto blockSize = pField2->iValue;
	if (blockSize  < 1 || blockSize  > 128)
		return -1;
	
	auto pField3 = (*knobInternal)["SubtractedConstant"];
	if (pField3.get() == nullptr)
		return -1;
	auto subtractedConstant = pField3->iValue;
	if (subtractedConstant  < -5 || subtractedConstant  > 5)
		return -1;

	if (checkResized(inImage, outImage) == -1)
		return -1;
	cv::AdaptiveThresholdTypes a;
	
	cv::adaptiveThreshold(inimage, outimage, 255, adaptiveMethod, thresholdType, blockSize, subtractedConstant);
	return 0;
};


int ThresholdCallback(std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> inImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::ImageResult> outImage, std::shared_ptr<CWindowsOpenCVComputeDLL::CContour::KnobInternalMap> knobInternal)
{
	auto inimage = *(inImage->pImage);
	auto outimage = *(outImage->pImage);

	if (inImage->pImage->cols != outImage->pImage->cols || inImage->pImage->rows != outImage->pImage->rows)
	{
		outImage->pImage->create(inImage->pImage->rows, inImage->pImage->cols, outImage->pImage->type());
	}

	if (checkResized(inImage, outImage) == -1)
		return -1;

	cv::threshold(inimage, outimage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	return 0;
};