#pragma once

#include "declarations.h"
#include "faceinfo.h"

#include <opencv2/core/core.hpp>
#include <vector>
#include <memory>
#include <map>


class FaceDetector
{
public:
	FaceDetector(std::shared_ptr<FaceDetectorConfig> p_pConfig);
	~FaceDetector();

	enum class ImageType : int
	{
		ORIGINAL_IMAGE = 0,
		SKIN_THRESHOLD = 1,
		HAIR_THRESHOLD = 2,
		DEFAULT = 0
	};

	// Class representing the result of detection procedure.
	class Result
	{
	public:
		// Vector holding structures representing the faces.
		std::vector<std::shared_ptr<FaceInfo>> faces;

		// Map holding matrices representing various stages of image processing.
		std::map<ImageType, std::shared_ptr<cv::Mat>> m_vPartialResults;
	};

	// Detection procedure
	std::shared_ptr<Result> detect(std::shared_ptr<const cv::Mat> image) const;

private:
	// Configuration of detector.
	std::shared_ptr<FaceDetectorConfig> m_pConfig;

	cv::Mat detectSkinColor(std::shared_ptr<const cv::Mat> image) const;

	cv::Mat detectSkinColor2(std::shared_ptr<const cv::Mat> image) const;

	cv::Mat getContourMask(std::shared_ptr<const cv::Mat> image) const;

	cv::Mat detectHair(std::shared_ptr<const cv::Mat> image) const;

	cv::Mat quantizeMask(const cv::Mat& img, int size = 5, int threshold = 12) const;

	cv::Mat quantizeMask2(const cv::Mat& img, int size = 5, int threshold = 12) const;

	cv::Mat clusterColors(const cv::Mat& img) const;
};
