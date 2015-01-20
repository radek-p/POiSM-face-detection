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
	std::unique_ptr<Result> detect(std::shared_ptr<const cv::Mat> image) const;

private:
	// Configuration of detector.
	std::shared_ptr<FaceDetectorConfig> m_pConfig;
};
