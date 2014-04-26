#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define SEG_OUTER_BG 10

class WatershedSegmenter
{
private:
	cv::Mat markers;

public:

	WatershedSegmenter(void);
	~WatershedSegmenter(void);

	void createMarkersFromImage(cv::Mat &image);
	
	cv::Mat findSegments(cv::Mat &image);

	void getSegmentedMask(const cv::Mat &segmentedImage, cv::Mat &outputImage);

	void mergeSegmentedMaskAndOriginalImage(cv::Mat &originalImage, cv::Mat &dest, cv::Mat &segmentedImage, cv::Mat &mask);
};

