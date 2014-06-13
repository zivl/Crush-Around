#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include <android/log.h>

#include "Core/LcObjectDetector.h"
#include "Core/VideoTracking.hpp"

using namespace std;
using namespace cv;

bool initialize = false;
std::vector<std::vector<cv::Point> > contours;

VideoTracking tracker;
extern "C"
{
	JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv*, jobject, jlong addrRgba, jint doInit);

	JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv* env, jobject, jlong addrRgba, jint doInit)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "Tomer","Entering JNI");

		if(doInit==1)
			__android_log_print(ANDROID_LOG_ERROR, "Tomer","doinit: %d", 1);//(ANDROID_LOG_ERROR, "Tomer", "In JNI Call doInit=%d", doInit);
		else if(doInit == 0)
			__android_log_print(ANDROID_LOG_ERROR, "Tomer","doinit: %d", 0);

		//Mat& mGr  = *(Mat*)addrGray;
		Mat& mRgba = *(Mat*)addrRgba;

		//Mat mRgb;

		//cv::cvtColor(mRgba, mRgb, CV_BGRA2BGR);
		if (!initialize)
		{
			__android_log_write(ANDROID_LOG_ERROR, "Tomer", "Not initialized");
			if (doInit == 1) {
				__android_log_write(ANDROID_LOG_ERROR, "Tomer", "Initializing");
				tracker.setReferenceFrame(mRgba);
				tracker.getWorld()->setObjectsToBeModeled(contours);
				tracker.prepareInPaintedScene(mRgba, contours);
				initialize = true;
				__android_log_write(ANDROID_LOG_ERROR, "Tomer", "Now initialized");
			} else {
				__android_log_write(ANDROID_LOG_ERROR, "Tomer", "Object detection");
				LcObjectDetector detect;
				//detect.setObjectSimplification(true);
				contours = detect.getObjectContours(mRgba);

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
					fillPoly(mRgba, ppt, npt, 1, Scalar(120, 250, 50));

					delete[] pnts;
				}
			}
		}
		else
		{
			//__android_log_write(ANDROID_LOG_ERROR, "Tomer", "AR Processing");
			//__android_log_print(ANDROID_LOG_ERROR, "Tomer", "AR Processing MAT (rows:%d, cols:%d)", mRgb.rows, mRgb.cols);
			tracker.processFrame(mRgba, mRgba);
		}

		//cv::cvtColor(mRgb, mRgba, CV_BGR2BGRA);
	}
}
