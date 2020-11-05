#include "CameraPosition.h"
#include <opencv2/imgproc/imgproc_c.h>
#include <math.h>
using namespace cv;

std::vector<Point2f> CameraPosition::findRectangle(Mat& image)
{
	Mat blurred(image);// blur will enhance edge detection
	medianBlur(image, blurred, 9);

	Mat gray, gray0(blurred.size(), CV_8U);
	std::vector<std::vector<Point>> contours;
	std::vector<std::vector<Point2f>> rectangles;

	// find rectangles in every color plane of the image
	for (int c = 0; c < colorPlane; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&blurred, 1, &gray0, 1, ch, 1);

		for (int l = 0; l < threshold_level; l++)
		{
			// Use Canny instead of zero threshold level!
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				Canny(gray0, gray, lowThreshold, highThreshold); // apply Canny. Take the upper threshold from slider and set the lower to 0 (which forces edges merging)
				dilate(gray, gray, Mat(), Point(-1, -1));// Dilate helps to remove potential holes between edge segments
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / threshold_level;
			}

			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);// Find contours and store them in a list

			// Test contours
			std::vector<Point2f> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

				//square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 1000 &&
					isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j <= 4; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					if (maxCosine < 0.3) rectangles.push_back(approx);
				}
			}
		}
	}

	return getBigRectangles(rectangles);
}


double CameraPosition::angle(Point2f pt1, Point2f pt2, Point2f pt0)
{
	double
		dx1 = pt1.x - pt0.x,
		dy1 = pt1.y - pt0.y,
		dx2 = pt2.x - pt0.x,
		dy2 = pt2.y - pt0.y;

	return (dx1 * dx2 + dy1 * dy2)
		/ sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

std::vector<Point2f> CameraPosition::getBigRectangles(const std::vector<std::vector<Point2f>>& rectangles)
{

	if (!rectangles.size()) return std::vector<Point2f>();

	int maxWidth = 0, maxHeight = 0, rectId = 0;

	for (size_t i = 0; i < rectangles.size(); i++)
	{
		// Convert a set of 4 unordered Points into a meaningful cv::Rect structure.
		Rect rectangle = boundingRect(Mat(rectangles[i]));

		// Store the index position of the biggest square found
		if ((rectangle.width >= maxWidth) && (rectangle.height >= maxHeight))
		{
			maxWidth = rectangle.width;
			maxHeight = rectangle.height;
			rectId = i;
		}
	}

	return rectangles[rectId];
}