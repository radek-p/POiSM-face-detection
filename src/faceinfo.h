#pragma once

#include <opencv2/core/core.hpp>


class FaceInfo
{
public:
	FaceInfo();
	~FaceInfo();

	cv::Rect m_oBoundingRect;
};
