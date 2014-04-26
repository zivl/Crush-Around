#include "WatershedSegmenter.h"


WatershedSegmenter::WatershedSegmenter(void)
{
}


WatershedSegmenter::~WatershedSegmenter(void)
{
}

void WatershedSegmenter::setMarkers(cv::Mat& markerImage)
{
    markerImage.convertTo(markers, CV_32S);
}

cv::Mat WatershedSegmenter::findSegments(cv::Mat& image)
{
    watershed(image, markers);
    markers.convertTo(markers,CV_8U);
    return markers;
}

