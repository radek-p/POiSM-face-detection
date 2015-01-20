#include "facedetector.h"

#include "faceinfo.h"

#include <iostream>


FaceDetector::FaceDetector(std::shared_ptr<FaceDetectorConfig> p_pConfig) :
	m_pConfig(p_pConfig)
{

}

FaceDetector::~FaceDetector()
{

}

std::unique_ptr<FaceDetector::Result> FaceDetector::detect(std::shared_ptr<const cv::Mat> image) const
{
	std::unique_ptr<Result> res = std::unique_ptr<Result>(new Result);

	std::cout << "DETECT START" << std::endl;

	std::shared_ptr<FaceInfo> pInfo = std::shared_ptr<FaceInfo>(new FaceInfo());
	pInfo->m_oBoundingRect = cv::Rect();
	pInfo->m_oBoundingRect.x = 100;
	pInfo->m_oBoundingRect.y = 110;
	pInfo->m_oBoundingRect.width = 200;
	pInfo->m_oBoundingRect.height = 220;

	std::cout << "HERE" << std::endl;
	std::shared_ptr<cv::Mat> mat = std::shared_ptr<cv::Mat>(new cv::Mat(cv::Mat::zeros(image->size(), CV_8UC3)));
	res->m_vPartialResults[ImageType::SKIN_THRESHOLD] = mat;

	std::cout << res->faces.size() << std::endl;
	res->faces.push_back(pInfo);

	return res;
}
