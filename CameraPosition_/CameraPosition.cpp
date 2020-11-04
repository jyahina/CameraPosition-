#include "CameraPosition.h"

using namespace cv;
// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle(Point2f pt1, Point2f pt2, Point2f pt0)
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
void CameraPosition::findRectangle(Mat& image, Mat &gray, std::vector<std::vector<Point2f> >& squares)
{
	std::cout <<std::endl<< "I'am here!!!";

	// blur will enhance edge detection
	Mat blurred(image);
	medianBlur(image, blurred, 9);

	Mat gray0(blurred.size(), CV_8U);
	std::vector<std::vector<Point>> contours;
//cvtColor(image, gray, cv::COLOR_BGR2GRAY); //new
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
			std::vector<Point2f> approx;
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


// returns sequence of squares detected on the image.
void CameraPosition::findSquares(const Mat& image, std::vector<std::vector<Point> >& squares)
{

	int thresh = 50, N = 11;
	squares.clear();
	Mat pyr, timg, gray0(image.size(), CV_8U), gray;
	// down-scale and upscale the image to filter out the noise
	pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
	pyrUp(pyr, timg, image.size());
	std::vector<std::vector<Point> > contours;
	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);
		// try several threshold levels
		for (int l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				Canny(gray0, gray, 0, thresh, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				dilate(gray, gray, Mat(), Point(-1, -1));
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				gray = gray0 >= (l + 1) * 255 / N;
			}
			// find contours and store them all as a list
			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
			std::vector<Point> approx;
			// test each contour
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);
				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(contourArea(approx)) > 1000 &&
					isContourConvex(approx))
				{
					double maxCosine = 0;
					for (int j = 2; j < 5; j++)
					{
						// find the maximum cosine of the angle between joint edges
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}
