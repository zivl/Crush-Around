#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "clipper.hpp"

class LcObjectDetector
{
public:
    LcObjectDetector(void);
    ~LcObjectDetector(void);

    std::vector<std::vector<cv::Point> > getObjectContours(const cv::Mat &image);

    ClipperLib::Paths simplify(ClipperLib::Paths &polygons);

private: 
    std::vector<ClipperLib::IntPoint> convertToClipperFormat(const std::vector<cv::Point> &poly);
};

