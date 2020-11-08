#include "CameraPosition.h"

using namespace cv;

CameraPosition::CameraPosition(const char * fileName)
{
	calculate(fileName);
}

cv::Mat CameraPosition::getRotationVector()
{
	return rvec;
}

cv::Mat CameraPosition::getTranslationVector()
{
	return tvec;
}

cv::Mat CameraPosition::getCameraPosition()
{
	return cameraPosition;
}

cv::Vec3d CameraPosition::getCameraPositionByEulerAngle()
{
	return cameraEulerAngle;
}

void CameraPosition::calculate(const char * fileName)
{
	cv::Mat cameraMatrix(3, 3, cv::DataType<double>::type, { 0.0 });
	cv::Mat distCoeffs(4, 1, cv::DataType<double>::type, { 0.0 });
	Mat rotCamerMatrix(3, 3, DataType<double>::type, { 0.0 });
	cv::setIdentity(cameraMatrix);

	auto image = getImage(fileName);
	auto rectangle = findRectangle(image);
	drawRectangle(image, rectangle);

	
	solvePnP(Generate3DPoints(), rectangle, cameraMatrix, distCoeffs, rvec, tvec, false);// calculate pose
	Rodrigues(rvec, rotCamerMatrix); //recalculation into rotation matrix

	cameraPosition = calcaulteCameraPosition(rotCamerMatrix, tvec);
	cameraEulerAngle = calculateEulerAngles(rotCamerMatrix, tvec);
}

void CameraPosition::drawRectangle(cv::Mat &image, std::vector<cv::Point2f> &square)
{
	for (int r = 0; r < 4; r++)
	{
		line(image, square[r], square[(r + 1) % 4], Scalar(0, 255, 0), 3, 8);
	}

	imshow(wndname, image);
	waitKey(0);
	destroyAllWindows();
}

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

	if (!rectangles.size())
		throw std::exception("Couldn't find a paper.");

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

Mat CameraPosition::calcaulteCameraPosition(const Mat &rotCamerMatrix, const Mat& translationVector)
{
	return -rotCamerMatrix.t() * translationVector;
}

Vec3d CameraPosition::calculateEulerAngles(const Mat & rotCamerMatrix, const Mat& translationVector)
{
	Vec3d eulerAngles;
	cv::Mat pose_mat = cv::Mat(3, 4, CV_64FC1); 
	cv::Mat out_intrinsics = cv::Mat(3, 3, CV_64FC1);
	cv::Mat out_rotation = cv::Mat(3, 3, CV_64FC1);
	cv::Mat out_translation = cv::Mat(3, 1, CV_64FC1);

	hconcat(rotCamerMatrix, translationVector, pose_mat);
	decomposeProjectionMatrix(pose_mat,
		out_intrinsics, out_rotation, out_translation,
		noArray(), noArray(), noArray(),
		eulerAngles);


	return eulerAngles;
}

std::vector<Point2f> CameraPosition::getBigRectangles(const std::vector<std::vector<Point2f>>& rectangles)
{

	int maxWidth = 0, maxHeight = 0;
	size_t rectId = 0;

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

Mat CameraPosition::getImage(const char* fileName)
{
	auto name = samples::findFile(fileName);
	Mat image = imread(name, IMREAD_COLOR);

	if (image.empty())
		throw std::exception("Couldn't load file.");

	return image;
}

std::vector<cv::Point3f> CameraPosition::Generate3DPoints()
{
	std::vector<cv::Point3f> points;

	points.push_back(cv::Point3f(.5, .5, -.5));
	points.push_back(cv::Point3f(.5, .5, .5));
	points.push_back(cv::Point3f(-.5, .5, .5));
	points.push_back(cv::Point3f(-.5, .5, -.5));

	return points;
}
