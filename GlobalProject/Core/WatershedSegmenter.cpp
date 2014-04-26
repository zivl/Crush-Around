#include "WatershedSegmenter.h"


WatershedSegmenter::WatershedSegmenter(void)
{
}


WatershedSegmenter::~WatershedSegmenter(void)
{
}

void WatershedSegmenter::createMarkersFromImage(cv::Mat& image)
{
    // Create markers image
	cv::Mat markerImage(image.size(),CV_8U,cv::Scalar(-1));

    //top rectangle
    markerImage(cv::Rect(0,0,image.cols, SEG_OUTER_BG)) = cv::Scalar::all(1);
    //bottom rectangle
    markerImage(cv::Rect(0,image.rows - SEG_OUTER_BG,image.cols, SEG_OUTER_BG)) = cv::Scalar::all(1);
    //left rectangle
    markerImage(cv::Rect(0,0, SEG_OUTER_BG,image.rows)) = cv::Scalar::all(1);
    //right rectangle
    markerImage(cv::Rect(image.cols - SEG_OUTER_BG,0, SEG_OUTER_BG,image.rows)) = cv::Scalar::all(1);
    //centre rectangle
    int centreW = image.cols/4;
    int centreH = image.rows/4;
    markerImage(cv::Rect((image.cols/2)-(centreW/2),(image.rows/2)-(centreH/2), centreW, centreH)) = cv::Scalar::all(2);

    markerImage.convertTo(markerImage, CV_BGR2GRAY);

    markerImage.convertTo(markers, CV_32S);
}

cv::Mat WatershedSegmenter::findSegments(cv::Mat& image)
{
    watershed(image, markers);
    markers.convertTo(markers,CV_8U);
    return markers;
}

void WatershedSegmenter::getSegmentedMask(const cv::Mat &segmentedImage, cv::Mat &outputImage) {
    convertScaleAbs(segmentedImage, outputImage, 1, 0);
    threshold(outputImage, outputImage, 1, 255, cv::THRESH_BINARY);
}

void WatershedSegmenter::mergeSegmentedMaskAndOriginalImage(cv::Mat &originalImage, cv::Mat &dest, cv::Mat &segmentedImage, cv::Mat &mask) {
	bitwise_and(originalImage, originalImage, dest, mask);
    dest.convertTo(dest,CV_8U);
}

