
#include "LcInPaint.h"

using namespace cv;

void LcInPaint::prepareInpaint(const Mat input){
	input.copyTo(frame2);
	input.copyTo(frame3);

	contours = objDetector.getObjectContours(frame2);

	std::vector<Point> approx;
	// test each contour
	for( size_t i = 0; i < contours.size(); i++ )
	{
		Point* pnts = new Point[contours[i].size()];
		for (int j = 0; j < contours[i].size(); j++)
		{
			pnts[j] = contours[i][j];
		}

		const Point* ppt[1] = { pnts };
		int npt[] = { static_cast<int>(contours[i].size()) };
		fillPoly(frame3, ppt, npt, 1, Scalar(120, 250, 50));

		delete[] pnts;
	}
}

void LcInPaint::inpaint(const Mat input, Mat output){
	mask.create(input.rows, input.cols, CV_8UC1);
	mask = Scalar(0);
	std::vector<Point> approx;

	// test each contour
	for( size_t i = 0; i < contours.size(); i++ )
	{
		Point* pnts = new Point[contours[i].size()];
		for (int j = 0; j < contours[i].size(); j++)
		{
			pnts[j] = contours[i][j];
		}

		const Point* ppt[1] = { pnts };
		int npt[] = { static_cast<int>(contours[i].size()) };
		fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 250));

		delete[] pnts;
	}

	cv::inpaint(input, mask, output, 15, INPAINT_TELEA);
}