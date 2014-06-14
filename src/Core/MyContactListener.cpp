#include "MyContactListener.h"


MyContactListener::MyContactListener(void) : m_contacts()
{
}


MyContactListener::~MyContactListener(void)
{
}

// Handle begin contact event. 
// The data from contact is copied because b2Contact passed in is reused
void MyContactListener::BeginContact(b2Contact* contact) {
    
    if (!contact->IsTouching())
    {
        return;
    }
    
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);    

    b2Vec2 vel1 = contact->GetFixtureA()->GetBody()->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
    b2Vec2 vel2 = contact->GetFixtureB()->GetBody()->GetLinearVelocityFromWorldPoint( worldManifold.points[0] );
   
    
    b2Vec2* impactVelocityA = new b2Vec2(vel1);
    b2Vec2* impactVelocityB = new b2Vec2(vel2);

    b2Vec2* point = new b2Vec2(worldManifold.points[0]);

    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB(), point, impactVelocityA, impactVelocityB };
    this->m_contacts.push_back(myContact);       
}

// Handle end contact event.
void MyContactListener::EndContact(b2Contact* contact) {
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end()) {
        m_contacts.erase(pos);
    }
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) 
{
}
 
void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) 
{
}