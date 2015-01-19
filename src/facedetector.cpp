#include "facedetector.h"

#include "faceinfo.h"

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


FaceDetector::FaceDetector(std::shared_ptr<FaceDetectorConfig> p_pConfig) :
	m_pConfig(p_pConfig)
{

}

FaceDetector::~FaceDetector()
{

}

std::shared_ptr<FaceDetector::Result> FaceDetector::detect(std::shared_ptr<const cv::Mat> image) const
{
	shared_ptr<Result> res = shared_ptr<Result>(new Result);
	cout << "DETECT START" << endl;

	Mat original = *image, skin3C;
	Mat contourMask = getContourMask(image);
	Mat skin1C = detectSkinColor2(image);
	Mat skin1C2 = detectSkinColor(image);
	skin1C &= contourMask;
	skin1C &= skin1C2;
//	Mat tmp = quantizeMask2(skin1C, 10, 50);
//	skin1C = tmp;
	Mat smallElement = getStructuringElement(MORPH_ELLIPSE, Size(5, 5), Point(2, 2));
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(15, 15), Point(7, 7));
	dilate(skin1C, skin1C, smallElement);
	erode(skin1C, skin1C, element);
	dilate(skin1C, skin1C, element);
	dilate(skin1C, skin1C, element);
	erode(skin1C, skin1C, element);
	erode(skin1C, skin1C, element);
	dilate(skin1C, skin1C, element);
	dilate(skin1C, skin1C, element);
	dilate(skin1C, skin1C, element);

//	namedWindow("skin", CV_WINDOW_NORMAL);
//	imshow("skin", skin1C);

	cvtColor(skin1C, skin3C, CV_GRAY2BGR);
	res->m_vPartialResults[ImageType::SKIN_THRESHOLD] = shared_ptr<Mat>(new Mat(skin3C));

//	Mat hair1C = detectHair(image), hair3C;
//	bitwise_not(contourMask, contourMask);
//	erode(contourMask, contourMask, element);
//	erode(contourMask, contourMask, element);
//	erode(contourMask, contourMask, smallElement);
//	dilate(contourMask, contourMask, element);
//	dilate(contourMask, contourMask, element);
//	hair1C |= contourMask;
//	tmp = quantizeMask2(hair1C),
//	hair1C = tmp;
//	dilate(hair1C, hair1C, element);

//	cvtColor(hair1C, hair3C, CV_GRAY2BGR);
//	res->m_vPartialResults[ImageType::HAIR_THRESHOLD] = shared_ptr<Mat>(new Mat(hair3C));

	// Extract faces

	vector<vector<Point>> faceContours;
	findContours(skin1C, faceContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	for (size_t i = 0; i < faceContours.size(); ++i)
	{
		double area = contourArea(faceContours[i]);
		double perimater = arcLength(faceContours[i], true);

		Scalar color1(255, 0, 0);
		Scalar color2(0, 0, 255);
		bool use1 = false;
		double a2p = area / perimater;

		if (20000 < area && area < 90000 && 35 < a2p)
			use1 = true;

		if (!use1)
			continue;

		drawContours(original, faceContours, i, (use1) ? color1 : color2, 5);

		std::shared_ptr<FaceInfo> face =  std::shared_ptr<FaceInfo>(new FaceInfo());
		face->m_oBoundingRect = boundingRect(faceContours[i]);
		res->faces.push_back(face);
	}

//	vector<vector<Point>> hairContours;
//	findContours(hair1C, hairContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

//	for (size_t i = 0; i < hairContours.size(); ++i)
//	{
//		double area = contourArea(hairContours[i]);
//		double perimater = arcLength(hairContours[i], true);

//		Scalar color1(0, 255, 0);
//		Scalar color2(0, 0, 255);
//		bool use1 = false;
//		double a2p = area / perimater;

////		if (10000 < area && area < 90000 && 30 < a2p)
////			use1 = true;

//		drawContours(original, hairContours, i, (use1) ? color1 : color2, 5);
//	}

	return res;
}

Mat FaceDetector::detectSkinColor(std::shared_ptr<const Mat> image) const
{
	Mat img = *image, imgCrCb, res;

	cv::cvtColor(img, imgCrCb, CV_BGR2YCrCb);
	inRange(imgCrCb, cv::Scalar(80, 133, 77), cv::Scalar(255, 173, 127), res);

	return res;
}

Mat FaceDetector::detectSkinColor2(std::shared_ptr<const Mat> image) const
{
	Mat img = *image, imgHsv, mask = Mat::zeros(image->size(), CV_8UC1);

	cv::cvtColor(img, imgHsv, CV_BGR2HSV);

	auto f1 = [](float r) {
		return r * (r * -1.376 + 1.0743) + 0.2;
	};

	auto f2 = [](float r) {
		return r * (r * -0.776 + 0.5601) + 0.18;
	};

	auto w = [](float r, float g) {
		float rr = (r - 0.33);
		float gg = (g - 0.33);
		return rr * rr + gg * gg;
	};

	for (int y = 0; y < img.rows; ++y)
	for (int x = 0; x < img.cols; ++x)
	{
		Vec3b color = img.at<Vec3b>(y, x);

		float r = ((float)color[2]) / (color[0] + color[1] + color[2]);
		float g = ((float)color[1]) / (color[0] + color[1] + color[2]);

		int H = imgHsv.at<Vec3b>(y, x)[0];

		mask.at<uchar>(y, x) = (g < f1(r) && g > f2(r) && w(r, g) > 0.001 && (H > 120 || H <= 10)) ? 255 : 0;
	}

	return mask;
}

Mat FaceDetector::getContourMask(std::shared_ptr<const Mat> image) const
{
	Mat img = *image;
//	Mat mask1 = detectSkinColor2(image);

	Mat imgHsv;
	cvtColor(img, imgHsv, CV_BGR2HSV_FULL);

	Mat spl[3];
	split(imgHsv, spl);
	Mat imgHC1 = spl[0];
	equalizeHist(imgHC1, imgHC1);

	namedWindow("kanal Hue po normalizacji histogramu", CV_WINDOW_NORMAL);
	imshow("kanal Hue po normalizacji histogramu", imgHC1);

	Mat imgHC3;
	cvtColor(imgHC1, imgHC3, CV_GRAY2BGR);
	int K = 4;
	imgHC1 = clusterColors(imgHC3);

//	namedWindow("a", CV_WINDOW_NORMAL);
//	imshow("a", imgHC1 * 64);

	std::vector<std::vector<std::vector<Point>>> groupsOfContours;

	for (int i = 0; i < K; ++i)
	{
		Mat em;
		std::vector<std::vector<Point>> contours;
		threshold(imgHC1, em, i + 1, 255, CV_8UC1);
//		Mat  = em.clone();
//		inRange(imgHC1, i, i + 1, img);

//		imshow("a", img);
//		waitKey(1000);

		findContours(em, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		groupsOfContours.push_back(contours);
	}


	Mat mask2 = Mat::zeros(img.size(), CV_8UC1);
	bitwise_not(mask2, mask2);

	for (int i = 0; i < K; ++i)
	{
		for (size_t j = 0; j < groupsOfContours[i].size(); ++j)
			drawContours(mask2, groupsOfContours[i], j, Scalar(0), 25);
	}

//	namedWindow("mask", CV_WINDOW_NORMAL);
//	imshow("mask", mask1);
//	waitKey(1000);
//	imshow("mask", mask2);
//	waitKey(1000);

	return mask2;
}

Mat FaceDetector::detectHair(std::shared_ptr<const Mat> image) const
{
	Mat img = *image, imgHsv, mask = Mat::zeros(image->size(), CV_8UC1);

	cv::cvtColor(img, imgHsv, CV_BGR2HSV);

	for (int y = 0; y < img.rows; ++y)
	for (int x = 0; x < img.cols; ++x)
	{
		Vec3b color = img.at<Vec3b>(y, x);
		int B = color[0], G = color[1], R = color[2];

		int I = (R + G + B) / 3;
		int H = imgHsv.at<Vec3b>(y, x)[0];

		mask.at<uchar>(y, x) = ((I < 80 && (B - G < 15 || B - R < 15)) && (10 < H && H <= 20)) ? 255 : 0;
	}

	return mask;
}

Mat FaceDetector::quantizeMask(const Mat &img, int size, int threshold) const
{
	int Y = img.rows / size;
	int X = img.cols / size;

	Mat res(Y, X, CV_8UC1);

	for (int y = 0; y < Y; ++y)
	for (int x = 0; x < X; ++x)
	{
		int count = 0;
		for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			if (img.at<uchar>(y * size + i, x * size + j) < 128)
				++count;

		res.at<uchar>(y, x) = (count > threshold) ? 0 : 255;
	}

	return res;
}

Mat FaceDetector::quantizeMask2(const Mat &img, int size, int threshold) const
{
	int Y = img.rows / size;
	int X = img.cols / size;

	Mat res = Mat::zeros(img.size(), CV_8UC1);

	for (int y = 0; y < Y; ++y)
	for (int x = 0; x < X; ++x)
	{
		int count = 0;
		for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			if (res.at<uchar>(size * y + i, size * x + j) < 128)
				++count;


		if (count <= threshold)
		{
			for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j)
				res.at<uchar>(size * y + i, size * x + j) = 255;
		}
	}

	return res;
}

Mat FaceDetector::clusterColors(const Mat &img) const
{
	Mat src = img.clone();
	blur(src, src, Size(25, 25));

	Mat p = Mat::zeros(src.cols * src.rows, 5, CV_32F);
	Mat bestLabels, centers, clustered;
	vector<Mat> bgr;
	cv::split(src, bgr);

	for(int i=0; i<src.cols*src.rows; i++) {
		p.at<float>(i,0) = (i / src.cols) / src.rows;
		p.at<float>(i,1) = (i  %src.cols) / src.cols;
		p.at<float>(i,2) = bgr[0].data[i] / 255.0;
		p.at<float>(i,3) = bgr[1].data[i] / 255.0;
		p.at<float>(i,4) = bgr[2].data[i] / 255.0;
	}

	int K = 4;
	kmeans(p, K, bestLabels, TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, centers);

	int colors[K];

	for(int i = 0; i < K; i++) {
		colors[i] = i;
	}

	Mat result(src.size(), CV_8UC1);

	for (int i = 0; i < src.cols * src.rows; i++)
	{
		result.at<uchar>(i / src.cols, i % src.cols) = colors[bestLabels.at<int>(0, i)];
	}

	return result;
}
