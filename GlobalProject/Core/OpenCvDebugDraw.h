#pragma once

#include <Box2d/Common/b2Draw.h>
#include <opencv2/core/core.hpp>

class OpenCvDebugDraw  : public b2Draw
{
public:
    OpenCvDebugDraw(void);
    OpenCvDebugDraw(float ptmRatio);
    ~OpenCvDebugDraw(void);

    /// Draw a closed polygon provided in CCW order.
    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    /// Draw a solid closed polygon provided in CCW order.
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    /// Draw a circle.
    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    
    /// Draw a solid circle.
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
    
    /// Draw a line segment.
    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    virtual void DrawTransform(const b2Transform& xf);

    cv::Mat GetScene();
    void SetScene(cv::Mat scene);

    void SetPtmRatio(float ratio);

    void SetLineThickness(int thinkness);

private:
    cv::Mat m_scene;

    float m_ptmRatio;
    int m_thickness;
};

