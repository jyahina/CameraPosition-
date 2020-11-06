#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

#include <vector>

class CameraPosition
{
public:
	CameraPosition(const char* fileName);

	cv::Mat getRotationVector();

	cv::Mat getTranslationVector();

	cv::Mat getCameraPosition();

	cv::Vec3d getCameraPositionByEulerAngle();

private:

	cv::Mat rvec = cv::Mat(3, 1, cv::DataType<double>::type); // rotation vector
	cv::Mat tvec = cv::Mat(3, 1, cv::DataType<double>::type); // translation vector
	cv::Mat cameraPosition; // camera position
	cv::Vec3d cameraEulerAngle;
	
	/* variables for rectangle search */
	const char* wndname = "Square Detection Demo";
	const int threshold_level = 2;// several threshold levels
	const int colorPlane = 3; // color plane of the image
	const int lowThreshold = 10; // low threshold for Canny algorithm
	const int highThreshold = 80;// high threshold for Canny algorithm

	/*
	* main function for define rotation and translation
	*/
	void calculate(const char* fileName);

	/*
	* draws the found rectangle on the image 
	*/
	void drawRectangle(cv::Mat &image, std::vector<cv::Point2f> &square);

	/*
	* returns the rectangle found in the image using Canny's algorithm
	*/
	std::vector<cv::Point2f> findRectangle(cv::Mat& image);

	/* helper function:
	* finds a cosine of angle between vectors
	* from pt0->pt1 and from pt0->pt2 
	*/
	double angle(cv::Point2f pt1, cv::Point2f pt2, cv::Point2f pt0);

	/*
	* calculates camera position
	*/
	cv::Mat calcaulteCameraPosition(const cv::Mat &rotCameraMatrix, const cv::Mat& translationVector);

	/*
	* determines Euler angles based on camera position
	*/
	cv::Vec3d calculateEulerAngles(const cv::Mat& rotCamerMatrix, const cv::Mat& translationVector);
	
	/*
	* finds the largest square out of many rectangles
	*/
	std::vector<cv::Point2f> getBigRectangles(const std::vector<std::vector<cv::Point2f>>& rectangles);

	/*
	* ñreates image by file name
	*/
	cv::Mat getImage(const char* fileName);

	/*
	* Defining the world coordinates for 3D points
	* Uses arbitrary values!!!
	*/
	std::vector<cv::Point3f> Generate3DPoints();
};

