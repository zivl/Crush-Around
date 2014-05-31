//
//  World.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#ifndef __DestroyAroundMe__World__
#define __DestroyAroundMe__World__

#include <Box2D/Box2D.h>
#include "Globals.h"
#include "OpenCvDebugDraw.h"
#include "MyContactListener.h"
#include "clipper.hpp"
#include "CVUtils.h"
#include "Observers/IBallHitObserver.h"

class World : IBallHitObserver {

public:
    World(int ballRadius);
    ~World();

    void initializeWorldOnFirstFrame(const cv::Mat& reference, const bool restrictBallToScene);

    void setObjectsToBeModeled(const std::vector<std::vector<cv::Point> > contours);

    void update(cv::Mat &refFrame);

    void createNewPhysicPointInWorld(const cv::Point2f point);

    void setDebugDrawEnabled(bool enabled);
    bool isDebugDrawEnabled();

    void updatePaddlesLocations(std::vector<cv::Point2f> points);

    void setBallRadius(double radius);
    double getBallRadius();

    OpenCvDebugDraw * getDebugDraw();

    b2World * getWorld();

    b2Body * getBallBody();
    std::vector<b2Body *> getObjectBodies();
    bool isAllObjectsDestroyed();

    std::vector<cv::Point2f*> getGuardLocations();
    std::vector<cv::Point*> getDestroyedPolygons();
    std::vector<int> getDestroyedPolygonsPointCount();

    // IBallHitObserver observer pattern methods for events
    virtual void attachBallHitObserver(std::function<void(float x, float y)> func);
    virtual void detachBallHitObserver(std::function<void(float x, float y)> func);
    virtual void notifyBallHitObservers(float x, float y);
    
private:

    double m_ballRadius;

    float dt;
    
    // flag indicating whether debug draw is enabled and should be used.
    bool m_debugDrawEnabled;

    // debug draw
    OpenCvDebugDraw* m_debugDraw;

    // Contact listener for colision response
    MyContactListener *m_contactListener;

    // the bodies of the object to be destroyed.
    std::vector<b2Body *>m_objectBodies;

    // list of "guards" position
    std::vector<cv::Point2f*> m_guardLocations;

    // box2d "world" objects
    b2World * m_world;
    b2Body * m_ballBody;
    b2Fixture * m_ballFixture;
    b2Body * m_groundBody;
    b2Fixture * m_groundFixture;

    // a list of destroyed regions
    std::vector<cv::Point*> m_destroyedPolygons;
    std::vector<int> m_destroyedPolygonsPointCount;

    std::vector<std::function<void(float x, float y)>> ballHitObserversList;
    
    b2Body * m_paddlesBody;
};

#endif /* defined(__DestroyAroundMe__World__) */
