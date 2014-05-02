//
//  ViewController.h
//  OpenCVTest
//
//  Created by Ziv Levy on 3/2/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

//
// Prefix header for all source files of the 'VideoFilters' target in the 'VideoFilters' project
//

#import <Availability.h>

#ifndef __IPHONE_4_0
#warning "This project uses features only available in iOS SDK 4.0 and later."
#endif

#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#import <opencv2/core/core.hpp>
#import <opencv2/objdetect/objdetect.hpp>
#import <opencv2/features2d/features2d.hpp>
#import <opencv2/imgproc/imgproc.hpp>
#import <opencv2/video/tracking.hpp>
#import <opencv2/calib3d/calib3d.hpp>
#endif

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#endif

#import <opencv2/highgui/cap_ios.h>
using namespace cv;

@interface ViewController : UIViewController <CvVideoCameraDelegate, UIGestureRecognizerDelegate>
{
    IBOutlet UIImageView* imageView;
	CvVideoCamera *videoCamera;
}

@property (nonatomic, retain) CvVideoCamera* videoCamera;

@property (nonatomic, strong) UIFont * fontLarge;
@property (nonatomic, strong) UIFont * fontSmall;

@property (nonatomic, assign) NSInteger score;

// UI Elements
@property (nonatomic, weak) IBOutlet UIView * scoringPanel;
@property (nonatomic, weak) IBOutlet UIImageView * innerScorePanelBG;
@property (nonatomic, weak) IBOutlet UILabel * scoreLabel;
@property (nonatomic, weak) IBOutlet UILabel * scorePoints;

@property (nonatomic, weak) IBOutlet UIView * blowItUpPanel;
@property (nonatomic, weak) IBOutlet UILabel * blowItUpLabel;


-(void)onTap:(UITapGestureRecognizer *)recognizer;

-(IBAction)resetCameraFirstPositionButton:(id)sender;

-(IBAction)onBlowItUpButton:(id)sender;




@end
