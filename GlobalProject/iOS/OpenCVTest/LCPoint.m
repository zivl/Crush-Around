//
//  LCPoint.m
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/5/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import "LCPoint.h"

@implementation LCPoint

@synthesize x;
@synthesize y;

-(CGPoint)getCGPoint {
	return CGPointMake(self.x, self.y);
}

@end
