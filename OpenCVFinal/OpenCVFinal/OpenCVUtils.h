#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

class OpenCVUtils
{
public:
	static void getBinMask( const Mat& comMask, Mat& binMask );
};

