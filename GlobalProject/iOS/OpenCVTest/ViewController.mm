//
//  ViewController.m
//  OpenCVTest
//
//  Created by Ziv Levy on 3/2/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import "ViewController.h"
#import "Core/VideoTracking.hpp"
#include "WatershedSegmenter.h"

@interface ViewController ()

@end

@implementation ViewController
@synthesize videoCamera;

BOOL isFirst = YES;
Mat firstImage;
VideoTracking *track;

- (void)viewDidLoad
{
	[super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
	track = new VideoTracking();

    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView];
	self.videoCamera.delegate = self;
    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    self.videoCamera.defaultFPS = 30;
    self.videoCamera.grayscaleMode = NO;
	UITapGestureRecognizer *gestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
	[imageView addGestureRecognizer:gestureRecognizer];
	gestureRecognizer.delegate = self;
	[self.videoCamera start];
}

#pragma mark - Protocol CvVideoCameraDelegate

#ifdef __cplusplus
- (void)processImage:(Mat&)image;
{
	Mat image_copy;
    //cvtColor(image, image_copy, CV_BGRA2BGR);

	/*if(isFirst){
		firstImage = image_copy;
		track->setReferenceFrame(firstImage);
		isFirst = !isFirst;
	}
	else {
		track->processFrame(image_copy, image_copy);

	}*/
	//cv::Canny(image, image, 100, 500);
	//image = showWatershedSegmentation(image_copy);


//    cvtColor(image_copy, image, CV_BGR2BGRA);


}

#define SEG_OUTER_BG 10
Mat showWatershedSegmentation(Mat image)
{
    Mat blank(image.size(),CV_8U,Scalar(0xFF));
    Mat dest;

    // Create markers image
    Mat markers(image.size(),CV_8U,Scalar(-1));

    //top rectangle
    markers(cv::Rect(0,0,image.cols, SEG_OUTER_BG)) = Scalar::all(1);
    //bottom rectangle
    markers(cv::Rect(0,image.rows - SEG_OUTER_BG,image.cols, SEG_OUTER_BG)) = cv::Scalar::all(1);
    //left rectangle
    markers(cv::Rect(0,0, SEG_OUTER_BG,image.rows)) = Scalar::all(1);
    //right rectangle
    markers(cv::Rect(image.cols - SEG_OUTER_BG,0, SEG_OUTER_BG,image.rows)) = cv::Scalar::all(1);
    //centre rectangle
    int centreW = image.cols/4;
    int centreH = image.rows/4;
    markers(cv::Rect((image.cols/2)-(centreW/2),(image.rows/2)-(centreH/2), centreW, centreH)) = cv::Scalar::all(2);

    markers.convertTo(markers, CV_BGR2GRAY);

    //Create watershed segmentation object
    WatershedSegmenter segmenter;
    segmenter.setMarkers(markers);
	Mat wshedMask = segmenter.findSegments(image);
    Mat mask;
    convertScaleAbs(wshedMask, mask, 1, 0);
    threshold(mask, mask, 1, 255, THRESH_BINARY);
    bitwise_and(image, image, dest, mask);
    dest.convertTo(dest,CV_8U);

    return mask;
}

#endif

-(IBAction)resetCameraFirstPositionButton:(id)sender {
	[self.videoCamera stop];
	isFirst = YES;
	[self.videoCamera start];
}

-(void)onTap:(UITapGestureRecognizer *)recognizer {
	CGPoint location = [recognizer locationInView:imageView];
	track->onMouse(1, location.x, location.y, nil, nil);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
