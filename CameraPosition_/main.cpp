// CameraPosition_.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "CameraPosition.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;

const char* wndname = "Square Detection Demo";

static void help(const char* programName)
{
	std::cout <<
		"\nA program using pyramid scaling, Canny, contours and contour simplification\n"
		"to find squares in a list of default images \n"
		"Returns sequence of squares detected on the image.\n"
		"Call:\n"
		"./" << programName << " [file_name (optional)]\n";
}

Mat getImage(const char* fileName)
{
	Mat image;

	try
	{
		samples::addSamplesDataSearchPath("C:\\Users\\Julia\\source\\repos\\CameraPosition_\\CameraPosition-\\CameraPosition_\\image\\");
		auto name = samples::findFile(fileName);
		image = imread(name, IMREAD_COLOR);

	}
	catch (std::exception& e)
	{
		std::cout << "Couldn't load " << fileName << std::endl << ". Reason: " << e.what() << std::endl;
	}
	return image;
}

// Defining the world coordinates for 3D points
// Creating vector to store vectors of 3D points for each checkerboard image
std::vector<cv::Point3f> Generate3DPoints()
{
	std::vector<cv::Point3f> points;


	float x, y, z;

	x = .5; y = .5; z = -.5;
	points.push_back(cv::Point3f(x, y, z));

	x = .5; y = .5; z = .5;
	points.push_back(cv::Point3f(x, y, z));

	x = -.5; y = .5; z = .5;
	points.push_back(cv::Point3f(x, y, z));

	x = -.5; y = .5; z = -.5;
	points.push_back(cv::Point3f(x, y, z));

	return points;
}


int main(int argc, char** argv)
{
	//help(argv[0]);

	static const char* defaultFileNames[] =
	{ "1.jpg", "2.jpg", "3.jpg", "4.jpg" };

	//добавить возможность задать свои файлы

	auto position = CameraPosition();
	auto objpoints = Generate3DPoints();

	for (const auto& fileName : defaultFileNames)
	{
		auto image = getImage(fileName);

		if (image.empty()) continue;


		auto square = position.findRectangle(image);

		if (square.size())
		{
			for (int r = 0; r < 4; r++) {
				line(image, square[r], square[(r + 1) % 4], Scalar(0, 255, 0), 3, 8);
			}

			imshow(wndname, image);
			cv::Mat cameraMatrix(3, 3, cv::DataType<double>::type);
			cv::setIdentity(cameraMatrix);

			cv::Mat rvec(3, 1, cv::DataType<double>::type);
			cv::Mat tvec(3, 1, cv::DataType<double>::type);
			cv::Mat distCoeffs(4, 1, cv::DataType<double>::type);
			distCoeffs.at<double>(0) = 0;
			distCoeffs.at<double>(1) = 0;
			distCoeffs.at<double>(2) = 0;
			distCoeffs.at<double>(3) = 0;

			solvePnP(objpoints, square, cameraMatrix, distCoeffs, rvec, tvec, false);

			//	calibrateCamera(Generate3DPoints(), squares[0], image.size(),cameraMatrix, distCoeffs, rvec, tvec);

			double Rtmp[9] = { 0.0 };

			Mat R(3, 3, DataType<double>::type, Rtmp);

			Rodrigues(rvec, R); //пересчет углов поворота в матрицу поворота

			std::cout << "Rotation vector : " << rvec << std::endl;
			std::cout << "Translation vector : " << tvec << std::endl;


			int c = waitKey();
			if (c == 27)break;
		}


	}
}
