//
//  World.cpp
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#include "World.h"

#include <iostream>

World::World(int ballRadius) : 
    m_objectBodies()
{
    this->setBallRadius(ballRadius);
    dt = 1.0f/60.0f;
    
    // define world with gravity
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    this->m_world = new b2World(gravity);

    // set and enable debug drawing
    // TODO: make optional
    this->m_debugDrawEnabled = false;
    this->m_debugDraw = new OpenCvDebugDraw(PTM_RATIO);

    m_world->SetDebugDraw(this->m_debugDraw);
    this->m_debugDraw->SetFlags( b2Draw::e_shapeBit );

    this->m_contactListener = new MyContactListener();
    this->m_world->SetContactListener(this->m_contactListener);
}

World::~World(){
    delete m_world;
    m_ballBody = NULL;
    m_world = NULL;

    delete m_debugDraw;
    delete m_contactListener;
    m_guardLocations.clear();
    m_destroyedPolygons.clear();
    m_destroyedPolygonsPointCount.clear();
}

void World::setBallRadius(double radius)
{
    this->m_ballRadius = radius / PTM_RATIO;

    if (this->m_ballFixture)
    {
        ((b2CircleShape*)this->m_ballFixture->GetShape())->m_radius = this->m_ballRadius;
    }
}

double World::getBallRadius()
{
    return this->m_ballRadius * PTM_RATIO;
}

void World::setDebugDrawEnabled(bool enabled){
    this->m_debugDrawEnabled = enabled;
}

bool World::isDebugDrawEnabled(){
    return this->m_debugDrawEnabled;
}

OpenCvDebugDraw * World::getDebugDraw(){
    return this->m_debugDraw;
}

b2World * World::getWorld(){
    return this->m_world;
}

std::vector<b2Body *> World::getObjectBodies(){
    return this->m_objectBodies;
}

b2Body * World::getBallBody(){
    return this->m_ballBody;
}

std::vector<cv::Point2f*> World::getGuardLocations() {
    return this->m_guardLocations;
}

bool World::isAllObjectsDestroyed(){
    return this->m_objectBodies.size() == 0;
}

std::vector<cv::Point*> World::getDestroyedPolygons(){
    return this->m_destroyedPolygons;
}

std::vector<int> World::getDestroyedPolygonsPointCount(){
    return this->m_destroyedPolygonsPointCount;
}

void World::initializeWorldOnFirstFrame(const cv::Mat& reference, const bool restrictBallToScene){
    // following is box2d world/ball initialization

    if(restrictBallToScene) 
    {
        // Create edges around the entire screen
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(0,0);

        cv::Size visibleSize(reference.cols, reference.rows);

        this->m_groundBody = m_world->CreateBody(&groundBodyDef);
        b2EdgeShape groundEdge;
        b2FixtureDef boxShapeDef;
        boxShapeDef.shape = &groundEdge;

        // wall definitions - bottom
        groundEdge.Set(b2Vec2(0,0), b2Vec2(visibleSize.width/PTM_RATIO, 0));
        this->m_groundBody->CreateFixture(&boxShapeDef);

        // left
        groundEdge.Set(b2Vec2(0,0), b2Vec2(0, visibleSize.height/PTM_RATIO));
        this->m_groundBody->CreateFixture(&boxShapeDef);

        // top
        groundEdge.Set(b2Vec2(0, visibleSize.height/PTM_RATIO),
            b2Vec2(visibleSize.width/PTM_RATIO, visibleSize.height/PTM_RATIO));
        this->m_groundBody->CreateFixture(&boxShapeDef);

        // right
        groundEdge.Set(b2Vec2(visibleSize.width/PTM_RATIO, visibleSize.height/PTM_RATIO),
            b2Vec2(visibleSize.width/PTM_RATIO, 0));
        this->m_groundBody->CreateFixture(&boxShapeDef);
    }

    // Create ball body and shape
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(30 / PTM_RATIO, 30 / PTM_RATIO);
    m_ballBody = m_world->CreateBody(&ballBodyDef);

    b2CircleShape circle;
    circle.m_radius = this->m_ballRadius;

    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 4.5f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = 1.0f;
    m_ballFixture = m_ballBody->CreateFixture(&ballShapeDef);

    m_ballBody->ApplyLinearImpulse(b2Vec2(30, 30), m_ballBody->GetPosition(), true);
}

// Set the objects (defined by contour points) to be models in the world and scene.
void World::setObjectsToBeModeled(const std::vector<std::vector<cv::Point>> contours) {

    int contourSize = (int)contours.size();
    for(int i = 0; i < contourSize; i++ )
    {
        std::vector<cv::Point> currentShape = contours[i];
        int numOfPoints = (int)currentShape.size();

        b2Vec2 * vertices = new b2Vec2[numOfPoints];
        ClipperLib::Paths* polygons = new ClipperLib::Paths();
        ClipperLib::Path polygon;

        for (int j = 0; j < numOfPoints; j++)
        {
            vertices[j].x = currentShape[j].x / PTM_RATIO;
            vertices[j].y = currentShape[j].y / PTM_RATIO;

            //cv::line(m_scene, currentShape[j], currentShape[(j + 1) % numOfPoints], cv::Scalar(0,0,255));
            //std::cout << "[" << vertices[j].x << "," <<vertices[j].y << "]" << std::endl;

            polygon.push_back(ClipperLib::IntPoint(currentShape[j].x, currentShape[j].y));
        }

        b2BodyDef objectBodyDef;
        objectBodyDef.type = b2_staticBody;

        b2Body *objectBody = m_world->CreateBody(&objectBodyDef);
        objectBody->SetUserData(polygons);

        polygons->push_back(polygon);

        b2EdgeShape objectEdgeShape;
        b2FixtureDef objectShapeDef;
        objectShapeDef.shape = &objectEdgeShape;

        for (int j = 0; j < numOfPoints - 1; j++)
        {
            objectEdgeShape.Set(vertices[j], vertices[j+1]);
            objectBody->CreateFixture(&objectShapeDef);
        }

        objectEdgeShape.Set(vertices[numOfPoints - 1], vertices[0]);
        objectBody->CreateFixture(&objectShapeDef);
        m_objectBodies.push_back(objectBody);
        delete[] vertices;
    }
}

void World::updatePaddlesLocations(std::vector<cv::Point2f> points)
{    
    if (!m_paddlesBody)
    {
        // Create edges around the entire screen
        b2BodyDef paddleBodyDef;
        paddleBodyDef.position.Set(0,0);

        this->m_paddlesBody = m_world->CreateBody(&paddleBodyDef);        
    }
    else
    {
		if(this->m_paddlesBody->GetFixtureList())
        {
			for (b2Fixture* f = this->m_paddlesBody->GetFixtureList(); f; )
			{
				b2Fixture* fixtureToDestroy = f;
				f = f->GetNext();
				this->m_paddlesBody->DestroyFixture( fixtureToDestroy );
			}
		}
    }

    // now add the paddles in their transformed location
    b2EdgeShape paddleEdgde;
    b2FixtureDef paddleShape;
    paddleShape.shape = &paddleEdgde;

    std::cout << points[0] << " - " << points[1] << std::endl;
        /*points[0].x << "," << points[0].y << " " << points[1].x << "," << points[1].y << " " <<
                 points[2].x << "," << points[2].y << "-" << points[3].x << "," << points[3].y << " " <<
                 points[4].x << "," << points[4].y << "-" << points[5].x << "," << points[5].y << " " <<
                 points[6].x << "," << points[6].y << "-" << points[7].x << "," << points[7].y << " " <<std::endl;*/

    for (size_t p = 0; p < points.size(); p+=2) {
        paddleEdgde.Set(b2Vec2(points[p].x / PTM_RATIO, points[p].y / PTM_RATIO), b2Vec2(points[p + 1].x / PTM_RATIO, points[p + 1].y / PTM_RATIO));
        this->m_paddlesBody->CreateFixture(&paddleShape);        
    }
}

void World::update(cv::Mat &homography)
{
    this->m_world->Step(dt, 10, 10);

    //check contacts
    std::vector<MyContact>::iterator pos;
    std::map<b2Body*, ClipperLib::Paths*> newBodyMap;
    std::vector<b2Body*> removeList;

    for(pos = this->m_contactListener->m_contacts.begin();
        pos != this->m_contactListener->m_contacts.end();
        ++pos)
    {
        MyContact contact = *pos;

        if ((contact.fixtureA == this->m_ballFixture || contact.fixtureB == this->m_ballFixture) 
            && (contact.fixtureA->GetBody() != m_groundBody && contact.fixtureB->GetBody() != m_groundBody)
            && (contact.fixtureA->GetBody() != m_paddlesBody && contact.fixtureB->GetBody() != m_paddlesBody))
        {
            b2Fixture* objectFixture = contact.fixtureA == this->m_ballFixture ? contact.fixtureB : contact.fixtureA;
            b2Body *objectBody = objectFixture->GetBody();

            if (objectFixture->GetType() == b2Shape::e_edge)
            {
                cv::Point2f hitPoint = CVUtils::transformPoint(cv::Point2f(contact.contactPoint->x * PTM_RATIO, contact.contactPoint->y * PTM_RATIO), homography);
                this->notifyBallHitObservers(hitPoint.x, hitPoint.y);

                // change the shape of the fixture
                // only go into processing if this body was not processed yet (possible ball hit two fixture of same body)
                if (newBodyMap.find(objectBody) == newBodyMap.end())
                {
                    ClipperLib::Paths* bodyPolygons = (ClipperLib::Paths*)objectBody->GetUserData();

                    b2Vec2* impactVelocity = contact.fixtureA == m_ballFixture ? contact.impactVelocityA : contact.impactVelocityB;
                    float ballAngle = atan2(impactVelocity->y, impactVelocity->x); // get the angle (in radians) the ball is moving to
                    float ballPower = impactVelocity->Length() * 0.5;    // get the "power" of the ball movement vector
                    float openingStepInRadians = 10 * CV_PI / 180;  // calculate the opening in radians

                    // create the clipping object/shape - a wedge from ball's center with 30 degree opening over ball direction (angle)
                    ClipperLib::Path clip;
                    clip.push_back(ClipperLib::IntPoint(contact.contactPoint->x * PTM_RATIO, contact.contactPoint->y * PTM_RATIO));

                    for(int step = 9; step > -10; step--)
                    {
                        float dx = cos(ballAngle + step * openingStepInRadians) * ballPower;
                        float dy = sin(ballAngle + step * openingStepInRadians) * ballPower;

                        clip.push_back(ClipperLib::IntPoint(contact.contactPoint->x * PTM_RATIO + dx, contact.contactPoint->y * PTM_RATIO + dy));
                    }

                    ClipperLib::Clipper clipper;
                    clipper.AddPaths((*bodyPolygons), ClipperLib::ptSubject, true);
                    clipper.AddPath(clip, ClipperLib::ptClip, true);

                    // the difference is the new polygon formed by the clipping (collision)
                    ClipperLib::Paths* newPolygons = new ClipperLib::Paths();
                    clipper.Execute(ClipperLib::ctDifference, (*newPolygons), ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                    // Save the new polygons of this body
                    objectBody->SetUserData(newPolygons);
                    newBodyMap[objectBody] = newPolygons;

                    // now, find the intersection regions - these should be inpainted to the scene
                    ClipperLib::Paths destroyedParts;
                    clipper.Execute(ClipperLib::ctIntersection, destroyedParts, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                    // paint the required areas to be coppied
                    for (size_t i = 0; i < destroyedParts.size(); i++)
                    {
                        cv::Point* points = new cv::Point[destroyedParts[i].size()];

                        for (int j = 0; j < destroyedParts[i].size(); j++)
                        {
                            points[j].x = (int)destroyedParts[i][j].X;
                            points[j].y = (int)destroyedParts[i][j].Y;
                        }

                        m_destroyedPolygons.push_back(points);
                        m_destroyedPolygonsPointCount.push_back((int)destroyedParts[i].size());
                    }
                }
            }
            else if (objectFixture->GetType() == b2Shape::e_circle)
            {
                removeList.push_back(objectBody);
            }
        }
    }

    std::map<b2Body*, ClipperLib::Paths*>::iterator iter;

    for(iter = newBodyMap.begin(); iter != newBodyMap.end(); iter++)
    {
        b2Body* objectBody = iter->first;
        ClipperLib::Paths* newPolygons = iter->second;

        // remove all the current fixtures from this body
        for (b2Fixture* f = objectBody->GetFixtureList(); f; )
        {
            b2Fixture* fixtureToDestroy = f;
            f = f->GetNext();
            objectBody->DestroyFixture( fixtureToDestroy );
        }

        if(newPolygons->size() == 0)
        {
            // there is no more pieces of the object left so remove it from list and world
            m_objectBodies.erase(std::find(m_objectBodies.begin(), m_objectBodies.end(), objectBody));
            m_world->DestroyBody(objectBody);   // TODO: better physics world cleanup
        }
        else
        {
            for (size_t i = 0; i < newPolygons->size(); i++)
            {
                b2EdgeShape objectEdgeShape;
                b2FixtureDef objectShapeDef;
                objectShapeDef.shape = &objectEdgeShape;

                ClipperLib::Path polygon = newPolygons->at(i);
                size_t j;
                for (j = 0; j < polygon.size() - 1; j++)
                {
                    objectEdgeShape.Set(b2Vec2(polygon[j].X / PTM_RATIO, polygon[j].Y / PTM_RATIO), b2Vec2(polygon[j+1].X / PTM_RATIO, polygon[j+1].Y / PTM_RATIO));
                    objectBody->CreateFixture(&objectShapeDef);
                }

                objectEdgeShape.Set(b2Vec2(polygon[j].X / PTM_RATIO, polygon[j].Y / PTM_RATIO), b2Vec2(polygon[0].X / PTM_RATIO, polygon[0].Y / PTM_RATIO));
                objectBody->CreateFixture(&objectShapeDef);
            }
        }
    }

    for (size_t i = 0; i < removeList.size(); i++){
        cv::Point2f* p = (cv::Point2f*)removeList[i]->GetUserData();

        std::vector<cv::Point2f*>::iterator position = std::find(m_guardLocations.begin(), m_guardLocations.end(), p);
        if (position != m_guardLocations.end()){ // == vector.end() means the element was not found
            m_guardLocations.erase(position);
        }

        removeList[i]->GetWorld()->DestroyBody(removeList[i]);
    }
}

void World::createNewPhysicPointInWorld(const cv::Point2f point){
    // add to the world model
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(point.x/PTM_RATIO, point.y/PTM_RATIO);
    //    std::cout << "[" << targetPoints[0].x/PTM_RATIO << "," <<targetPoints[0].y/PTM_RATIO << "]" << std::endl;
    b2Body *body = m_world->CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = 5.0 / PTM_RATIO;

    b2FixtureDef shapeDef;
    shapeDef.shape = &circle;
    shapeDef.density = 10.0f;
    shapeDef.friction = 0;
    body->CreateFixture(&shapeDef);

    // add the target point to the list of blacked areas
    cv::Point2f* p = new cv::Point2f(point);
    m_guardLocations.push_back(p);

    body->SetUserData(p);
}

void World::attachBallHitObserver(std::function<void(float x, float y)> func)
{
    ballHitObserversList.push_back(func);
}
void World::detachBallHitObserver(std::function<void(float x, float y)> func)
{
    //ballHitObserversList.erase(std::remove(ballHitObserversList.begin(), ballHitObserversList.end(), func), ballHitObserversList.end());
}

void World::notifyBallHitObservers(float x, float y)
{
    for(std::vector<std::function<void(float x, float y)>>::const_iterator iter = ballHitObserversList.begin(); iter != ballHitObserversList.end(); ++iter)
    {
        if(*iter != 0)
        {
            (*iter)(x, y);
        }
    }
}


