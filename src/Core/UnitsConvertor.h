#pragma once

#include <opencv2/core/core.hpp>
#include <Box2D\Box2D.h>
#include "clipper.hpp"

class UnitsConvertor
{
public:
    UnitsConvertor(void);
    ~UnitsConvertor(void);

    

    static ClipperLib::IntPoint b2Vec2ToClipperPoint(const b2Vec2 &point);
    static ClipperLib::IntPoint CvPointToClipperPoint(const cv::Point &point);

    static cv::Point b2Vec2ToCvPoint(const b2Vec2 &point);
    static cv::Point ClipperPointToCvPoint(const ClipperLib::IntPoint &point);

    static b2Vec2 CvPointToB2Vec2(const cv::Point &point);   
    static b2Vec2 ClipperPointToB2Vec2(const ClipperLib::IntPoint &point);
    

};

