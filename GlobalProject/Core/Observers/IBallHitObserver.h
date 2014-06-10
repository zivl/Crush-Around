//
//  IBallHitObserver.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/6/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//
// Observer class to raise event when the ball hits other object

#ifndef __DestroyAroundMe__IBallHitObserver__
#define __DestroyAroundMe__IBallHitObserver__

class IBallHitObserver {

public:
	virtual void attachBallHitObserver(std::function<void(float x, float y)> func){};
	virtual void detachBallHitObserver(std::function<void(float x, float y)> func){};
	virtual void notifyBallHitObservers(float x, float y){};

};

#endif /* defined(__DestroyAroundMe__IBallHitObserver__) */
