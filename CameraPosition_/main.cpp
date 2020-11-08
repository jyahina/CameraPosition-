// CameraPosition_.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "CameraPosition.h"

using namespace cv;

std::vector<const char*> fileNames;
std::vector<const char*> defaultFileNames = { ".\\image\\1.jpg", ".\\image\\2.jpg", ".\\image\\3.jpg", ".\\image\\4.jpg" };

static void help()
{
	std::string answer;

	std::cout <<
		"\nA program using pyramid scaling, Canny, contours and contour simplification\n"
		"to find squares in a list of default images (or given image). \n"
		"Returns data of the camera position and a rectangle detected on the image.\n"
		"\nWant to set your file (Y/N):\n";

	std::cin >> answer;

	if (answer == "Y") 
	{
		char _fileName[256];
		std::cout << "\nYour file:\n";
		std::cin >> _fileName;

		fileNames = { _fileName };
	}
	else if (answer == "N")
	{ 
		fileNames = defaultFileNames;
	}
	else
	{
		std::cout << "Wrong answer! The program will use the default images.";
		fileNames = defaultFileNames;
	}

}

int main(int argc, char** argv)
{
	help();

	for (const auto& fileName : fileNames)
	{
		std::cout << std::endl << "File:" << fileName << std::endl;

		try {
			CameraPosition camera(fileName);
			
			std::cout << "\tRotation vector : " << std::endl << camera.getRotationVector() << std::endl;
			std::cout << "\tTranslation vector : " << std::endl << camera.getTranslationVector() << std::endl;
			std::cout << "\t Camera Position: " << std::endl << camera.getCameraPosition() << std::endl;
			std::cout << "\t Euler Angles: " << std::endl << camera.getCameraPositionByEulerAngle() << std::endl;

		}
		catch (std::exception& e)
		{
			std::cout << "\n\nError: " << e.what();
		}
	}
}
