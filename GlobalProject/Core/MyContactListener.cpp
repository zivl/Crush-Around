#include "MyContactListener.h"


MyContactListener::MyContactListener(void) : _contacts()
{
}


MyContactListener::~MyContactListener(void)
{
}

void MyContactListener::BeginContact(b2Contact* contact) {
    // We need to copy out the data because the b2Contact passed in is reused.
    b2WorldManifold manifold;
    contact->GetWorldManifold(&manifold);

    b2Vec2* point = new b2Vec2(manifold.points[0]);

    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB(), point };
    this->_contacts.push_back(myContact);       
}

void MyContactListener::EndContact(b2Contact* contact) {
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(_contacts.begin(), _contacts.end(), myContact);
    if (pos != _contacts.end()) {
        _contacts.erase(pos);
    }
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
}
 
void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
}