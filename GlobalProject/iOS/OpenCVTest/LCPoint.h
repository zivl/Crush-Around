//
//  LCPoint.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/5/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LCPoint : NSObject

@property (nonatomic, assign) float x;
@property (nonatomic, assign) float y;

-(CGPoint)getCGPoint;

@end
