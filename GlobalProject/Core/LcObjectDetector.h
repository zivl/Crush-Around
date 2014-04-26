#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>

class LcObjectDetector
{
public:
    LcObjectDetector(void);
    ~LcObjectDetector(void);

    std::vector<std::vector<cv::Point>> getObjectContours(cv::Mat &image);
};

