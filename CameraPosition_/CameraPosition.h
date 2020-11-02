#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <vector>

struct Position
{
	Position();
	double x = 0.;
	double y = 0.;
	double z = 0.;
};

struct Orientation
{

};

class CameraPosition
{
public:
	CameraPosition();

	Position getCameraPosition();
	Orientation getCameraOrientation();
	void findRectangle(cv::Mat& image, std::vector<std::vector<cv::Point> >& squares);

private:


};

