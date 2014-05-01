#pragma once

#include "Box2D/Box2D.h"
#include <vector>
#include <algorithm>
 
// define a struct to keep contact information between two fixtures
struct MyContact {
    b2Fixture *fixtureA;
    b2Fixture *fixtureB;
    b2Vec2* contactPoint;
    b2Vec2* impactVelocityA;
    b2Vec2* impactVelocityB;


    bool operator==(const MyContact& other) const
    {
        return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
    }
};

class MyContactListener : public b2ContactListener 
{
public:
    std::vector<MyContact> m_contacts;

    MyContactListener(void);
    ~MyContactListener(void);

    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);    
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

