#pragma once

class WatershedSegmenter
{
private:
	cv::Mat markers;

public:

	WatershedSegmenter(void);
	~WatershedSegmenter(void);

    void setMarkers(cv::Mat &markerImage);
	
	cv::Mat findSegments(cv::Mat &image);
};

