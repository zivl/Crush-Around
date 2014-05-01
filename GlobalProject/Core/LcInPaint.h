

#ifndef __DestroyAroundMe__LcInPaint__
#define __DestroyAroundMe__LcInPaint__

#include "LcObjectDetector.h"

class LcInPaint {
private:
	LcObjectDetector objDetector;
	std::vector<std::vector<cv::Point>> contours;

	cv::Mat mask;

	cv::Mat frame2, frame3;

public:
	void prepareInpaint(const cv::Mat input);
	void inpaint(const cv::Mat input, cv::Mat output);
};

#endif /* defined(__DestroyAroundMe__LcInPaint__) */
