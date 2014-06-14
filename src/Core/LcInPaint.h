

#ifndef __DestroyAroundMe__LcInPaint__
#define __DestroyAroundMe__LcInPaint__

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/photo/photo.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "clipper.hpp"

#if defined _MSC_VER 
#include "Core/Globals.h"
#endif

class LcInPaint {
private:
	std::vector<std::vector<cv::Point> > contours;

public:
	void inpaint(const cv::Mat input, std::vector<std::vector<cv::Point> > contours, cv::Mat &output);
};

#endif /* defined(__DestroyAroundMe__LcInPaint__) */
