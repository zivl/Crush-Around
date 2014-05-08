//
//  IObjectsDestryedObserver.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/8/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#ifndef DestroyAroundMe_IObjectsDestryedObserver_h
#define DestroyAroundMe_IObjectsDestryedObserver_h

class IObjectsDestryedObserver {

public:
	virtual void attachObjectsDestryedObserver(std::function<void()> func){};
	virtual void detachObjectsDestryedObserver(std::function<void()> func){};
	virtual void notifyObjectsDestryedObservers(){};

};


#endif
