//
//  IBallInSceneObserver.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/8/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//
// Observer class to raise event when the ball goes out of the scene

#ifndef DestroyAroundMe_IBallInSceneObserver_h
#define DestroyAroundMe_IBallInSceneObserver_h

class IBallInSceneObserver {

public:
	virtual void attachBallInSceneObserver(std::function<void()> func){};
	virtual void detachBallInSceneObserver(std::function<void()> func){};
	virtual void notifyBallInSceneObservers(){};

};

#endif
