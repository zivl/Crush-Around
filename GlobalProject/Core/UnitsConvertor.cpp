#include "UnitsConvertor.h"


UnitsConvertor::UnitsConvertor(void)
{
}


UnitsConvertor::~UnitsConvertor(void)
{
}


ClipperLib::IntPoint UnitsConvertor::b2Vec2ToClipperPoint(const b2Vec2 &point)
{
    return ClipperLib::IntPoint(point.x, point.y);
}

ClipperLib::IntPoint UnitsConvertor::CvPointToClipperPoint(const cv::Point &point)
{
    return ClipperLib::IntPoint(point.x, point.y);
}

cv::Point UnitsConvertor::b2Vec2ToCvPoint(const b2Vec2 &point)
{
    return cv::Point(point.x, point.y);
}

cv::Point UnitsConvertor::ClipperPointToCvPoint(const ClipperLib::IntPoint &point)
{
    return cv::Point(point.X, point.Y);
}

b2Vec2 UnitsConvertor::CvPointToB2Vec2(const cv::Point &point)
{
    return b2Vec2(point.x, point.y);
}

b2Vec2 UnitsConvertor::ClipperPointToB2Vec2(const ClipperLib::IntPoint &point)
{
    return b2Vec2(point.X, point.Y);
}

