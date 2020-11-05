#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <vector>

class CameraPosition
{
public:
	CameraPosition() = default;

	/*
	* returns the rectangle found in the image using Canny's algorithm
	*/
	std::vector<cv::Point2f> findRectangle(cv::Mat& image);


private:

	const int threshold_level = 2;// several threshold levels
	const int colorPlane = 3; // color plane of the image
	const int lowThreshold = 10; // low threshold for Canny algorithm
	const int highThreshold = 80;// high threshold for Canny algorithm

	/* helper function:
	* finds a cosine of angle between vectors
	* from pt0->pt1 and from pt0->pt2 
	*/
	double angle(cv::Point2f pt1, cv::Point2f pt2, cv::Point2f pt0);

	/*
	* find the largest square out of many rectangles
	*/
	std::vector<cv::Point2f> getBigRectangles(const std::vector<std::vector<cv::Point2f>>& rectangles);
};

