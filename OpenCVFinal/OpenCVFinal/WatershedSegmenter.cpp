#include "WatershedSegmenter.h"


WatershedSegmenter::WatershedSegmenter(void)
{
}


WatershedSegmenter::~WatershedSegmenter(void)
{
}

void WatershedSegmenter::setMarkers(Mat& markerImage)
{
    markerImage.convertTo(markers, CV_32S);
}

Mat WatershedSegmenter::findSegments(Mat& image)
{
    watershed(image, markers);
    markers.convertTo(markers,CV_8U);
    return markers;
}

