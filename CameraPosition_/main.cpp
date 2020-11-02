// CameraPosition_.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "CameraPosition.h"

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
		samples::addSamplesDataSearchPath("C:\\Users\\Julia\\source\\repos\\CameraPosition_\\CameraPosition_\\image\\");
		auto name = samples::findFile(fileName);
		image = imread(name, IMREAD_COLOR);

	}
	catch (std::exception& e)
	{
		std::cout << "Couldn't load " << fileName << std::endl << ". Reason: " << e.what() << std::endl;
	}
	return image;
}

int main(int argc, char** argv)
{
	//help(argv[0]);

	static const char* defaultFileNames[] =
	{ "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg"};
	
	//добавить возможность задать свои файлы

	auto position = CameraPosition();
	
	for (const auto& fileName : defaultFileNames)
	{

		auto image = getImage(fileName);

		if (!image.empty())
		{
			std::vector<std::vector<Point>> squares;


			position.findRectangle(image, squares);
			polylines(image, squares, true, Scalar(0, 255, 0), 3, LINE_AA);
			imshow(wndname, image);

			int c = waitKey();
			if (c == 27)break;
		}

	}
}
