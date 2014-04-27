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
    
    b2WorldManifold manifold;
    contact->GetWorldManifold(&manifold);

    b2Vec2* point = new b2Vec2(manifold.points[0]);

    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB(), point };
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