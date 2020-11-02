#include "CameraPosition.h"

using namespace cv;
// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}
CameraPosition::CameraPosition()
{
}
void CameraPosition::findRectangle(Mat& image, std::vector<std::vector<Point> >& squares)
{
	std::cout <<std::endl<< "I'am here!!!";
	// blur will enhance edge detection
	Mat blurred(image);
	medianBlur(image, blurred, 9);

	Mat gray0(blurred.size(), CV_8U), gray;
	std::vector<std::vector<Point>> contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = {c, 0};
		mixChannels(&blurred, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		const int threshold_level = 2;
		for (int l = 0; l < threshold_level; l++)
		{
			// Use Canny instead of zero threshold level!
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				Canny(gray0, gray, 10, 20, 3); //

				// Dilate helps to remove potential holes between edge segments
				dilate(gray, gray, Mat(), Point(-1,-1));
			}
			else
			{
					gray = gray0 >= (l+1) * 255 / threshold_level;
			}

			// Find contours and store them in a list
			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			// Test contours
			std::vector<Point> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
					// approximate contour with accuracy proportional
					// to the contour perimeter
					approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

					// Note: absolute value of an area is used because
					// area may be positive or negative - in accordance with the
					// contour orientation
					if (approx.size() == 4 &&
							fabs(contourArea(Mat(approx))) > 1000 &&
							isContourConvex(Mat(approx)))
					{
							double maxCosine = 0;

							for (int j = 2; j < 5; j++)
							{
									double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
									maxCosine = MAX(maxCosine, cosine);
							}

							if (maxCosine < 0.3)
									squares.push_back(approx);
					}
			}
		}
	}
}
