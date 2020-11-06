// CameraPosition_.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CameraPosition.h"

using namespace cv;

static void help(const char* programName)
{
	std::cout <<
		"\nA program using pyramid scaling, Canny, contours and contour simplification\n"
		"to find squares in a list of default images \n"
		"Returns sequence of squares detected on the image.\n"
		"Call:\n"
		"./" << programName << " [file_name (optional)]\n";
}


int main(int argc, char** argv)
{
	//help(argv[0]);

	static const char* defaultFileNames[] =
	{ "1.jpg", "2.jpg", "3.jpg", "4.jpg" };

	//добавить возможность задать свои файлы

	for (const auto& fileName : defaultFileNames)
	{
		std::cout << "File:" << fileName << std::endl;

		try {
			CameraPosition camera(fileName);
			
			std::cout << "\tRotation vector : " << std::endl << camera.getRotationVector() << std::endl;
			std::cout << "\tTranslation vector : " << std::endl << camera.getTranslationVector() << std::endl;
			std::cout << "\t Camera Position: " << std::endl << camera.getCameraPosition() << std::endl;
			std::cout << "\t Euler Angles: " << std::endl << camera.getCameraPositionByEulerAngle() << std::endl;

		}
		catch (std::exception& e)
		{
			std::cout << "Error: " << e.what();
		}
	}
}
