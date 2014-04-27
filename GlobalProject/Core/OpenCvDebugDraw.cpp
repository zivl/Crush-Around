#include "OpenCvDebugDraw.h"

// Convert b2Vec2 to cv::Point
#define b2Vec2ToCvPoint(vec2) cv::Point(vec2.x * this->m_ptmRatio, vec2.y * this->m_ptmRatio);

// Convert b2Color to Open CV Scalar. Open CV colors are in BGR with opacity
#define b2ColorToCvScalar(color, opacity) cv::Scalar(color.b * 255, color.g * 255, color.r * 255, opacity);

// Convert b2Color to Open CV Scalar. Open CV colors are in BGR
//#define b2ColorToCvScalar(color) b2ColorToCvScalar(color, 255);

OpenCvDebugDraw::OpenCvDebugDraw(void)
{
    this->SetPtmRatio(1.0F / 32.0F);
    this->SetLineThickness(2);
}

OpenCvDebugDraw::OpenCvDebugDraw(float ratio)
{
    this->SetPtmRatio(ratio);
    this->SetLineThickness(2);
}


OpenCvDebugDraw::~OpenCvDebugDraw(void)
{
}

void OpenCvDebugDraw::SetScene(cv::Mat scene)
{
    this->m_scene = scene;
}


cv::Mat OpenCvDebugDraw::GetScene()
{
    return this->m_scene;
}

void OpenCvDebugDraw::SetPtmRatio(float ratio)
{
    this->m_ptmRatio = ratio;
}

void OpenCvDebugDraw::SetLineThickness(int thickness)
{
    this->m_thickness = thickness;
}

/// Draw a closed polygon provided in CCW order.
void OpenCvDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    // Draw the poligon on the member scene if not empty
    if (this->m_scene.empty())
    {
        return;
    }

    //Open CV colors are in BGR
    cv::Scalar colorInCv = b2ColorToCvScalar(color, 40);

    for(int idx = 0; idx < vertexCount; idx++)
    {
        cv::line(this->m_scene, 
                 cvPoint(vertices[idx].x * this->m_ptmRatio, vertices[idx].y * this->m_ptmRatio), 
                 cvPoint(vertices[idx + 1 % vertexCount].x * this->m_ptmRatio, vertices[idx + 1 % vertexCount].y * this->m_ptmRatio), 
                 colorInCv, 
                 this->m_thickness);
    }
}

/// Draw a solid closed polygon provided in CCW order.
void OpenCvDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    // Draw the poligon on the member scene if not empty
    if (this->m_scene.empty())
    {
        return;
    }

    cv::Point * pnts = (cv::Point*)malloc(sizeof(cv::Point) * vertexCount);

    for(int idx = 0; idx < vertexCount; idx++)
    {
        pnts[idx] = b2Vec2ToCvPoint(vertices[idx]);//cvPoint(vertices[idx].x * this->m_ptmRatio, vertices[idx].y * this->m_ptmRatio);
    }

    //Open CV colors are in BGR
    cv::Scalar colorInCv = b2ColorToCvScalar(color, 40);
    const cv::Point* ppt[1] = { pnts };
    int npt[] = { vertexCount };
    cv::fillPoly(this->m_scene, ppt, npt, 1, colorInCv);

    b2Color edgesColor(color.r * 0.8, color.g * 0.8, color.b * 0.8);

    this->DrawPolygon(vertices, vertexCount, edgesColor);
}

/// Draw a circle.
void OpenCvDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    cv::Scalar colorInCv = b2ColorToCvScalar(color, 40);
    cv::Point centerInCv = b2Vec2ToCvPoint(center);
    cv::circle(this->m_scene, centerInCv, radius * this->m_ptmRatio, colorInCv, this->m_thickness);
}
    
/// Draw a solid circle.
void OpenCvDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    cv::Scalar colorInCv = b2ColorToCvScalar(color, 40);
    cv::Point centerInCv = b2Vec2ToCvPoint(center);
    cv::circle(this->m_scene, centerInCv, radius * this->m_ptmRatio, colorInCv, -1);

    //TODO: draw the axis
    b2Color edgesColor(color.r * 0.8, color.g * 0.8, color.b * 0.8);
    this->DrawCircle(center, radius, edgesColor);
}
    
/// Draw a line segment.
void OpenCvDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    cv::Point p1Cv = b2Vec2ToCvPoint(p1);
    cv::Point p2Cv = b2Vec2ToCvPoint(p2);
    cv::Scalar colorInCv = b2ColorToCvScalar(color, 40);
    cv::line(this->m_scene, p1Cv, p2Cv, colorInCv, this->m_thickness);
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void OpenCvDebugDraw::DrawTransform(const b2Transform& xf)
{
    //TODO: implement - ???
}

