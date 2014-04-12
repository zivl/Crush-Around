#pragma once
#include "Segmenter.h"

class WatershedSegmenter : public Segmenter
{
private:
    Mat markers;

public:

	WatershedSegmenter(void);
	~WatershedSegmenter(void);

    void setMarkers(Mat &markerImage);
	
	Mat findSegments(Mat &image);
};

