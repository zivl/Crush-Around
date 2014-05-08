#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>

#include "Core/LcObjectDetector.h"
//#include "Core/VideoTracking.hpp"

using namespace std;
using namespace cv;

extern "C"
{
	JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);

	JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv* env, jobject, jlong addrGray, jlong addrRgba)
	{
		Mat& mGr  = *(Mat*)addrGray;
		Mat& mRgb = *(Mat*)addrRgba;

		LcObjectDetector detect;
		detect.setObjectSimplification(true);
		std::vector<std::vector<cv::Point> > contours = detect.getObjectContours(mRgb);

		// draw each contour on the image
		for( int i = 0; i < contours.size(); i++ )
		{
			Point* pnts = new Point[contours[i].size()];
			for (int j = 0; j < contours[i].size(); j++)
			{
				pnts[j] = contours[i][j];
			}

			const Point* ppt[1] = { pnts };
			int npt[] = { contours[i].size() };
			fillPoly(mRgb, ppt, npt, 1, Scalar(120, 250, 50));

			delete[] pnts;

			//VideoTracking tracker;
		}
	}
}
