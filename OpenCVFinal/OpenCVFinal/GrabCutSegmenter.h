#pragma once
#include "Definitions.h"
#include "Segmenter.h"
#include "OpenCVUtils.h"


const Scalar RED = Scalar(0,0,255);
const Scalar PINK = Scalar(230,130,255);
const Scalar BLUE = Scalar(255,0,0);
const Scalar LIGHTBLUE = Scalar(255,255,160);
const Scalar GREEN = Scalar(0,255,0);

const int BGD_KEY = EVENT_FLAG_CTRLKEY;
const int FGD_KEY = EVENT_FLAG_SHIFTKEY;

class GrabCutSegmenter : public Segmenter
{
private:
    void setRectInMask();
    void setLblsInMask( int flags, Point p, bool isPr );

    const string* winName;
    const Mat* image;
    Mat mask;
    Mat bgdModel, fgdModel;

    uchar rectState, lblsState, prLblsState;
    bool isInitialized;

    Rect rect;
    vector<Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
    int iterCount;

public:
	GrabCutSegmenter(void);
	~GrabCutSegmenter(void);
    
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
    static const int radius = GRAB_CUT_RADIUS;
    static const int thickness = GRAB_CUT_THICKNESS;

    void reset();
    void setImageAndWinName( const Mat& _image, const string& _winName );
    void showImage() const;
    void mouseClick( int event, int x, int y, int flags, void* param );
    int nextIter();
    int getIterCount() const { return iterCount; }
	Mat findSegments(Mat &image);

};

