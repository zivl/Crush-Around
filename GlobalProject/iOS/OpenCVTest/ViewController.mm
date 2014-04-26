//
//  ViewController.m
//  OpenCVTest
//
//  Created by Ziv Levy on 3/2/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import "ViewController.h"
#import "VideoTracking.hpp"
#include "WatershedSegmenter.h"
#include "LcObjectDetector.h"


@interface ViewController ()

@end

@implementation ViewController
@synthesize videoCamera;

BOOL isFirst = YES;
BOOL imageForSegmentationHasBeenTaken = NO;

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

std::vector<std::vector<cv::Point>> getLCDetection(const cv::Mat &image, cv::Mat &output){
	std::vector<std::vector<cv::Point>> contours;

    LcObjectDetector objDetector;

	contours = objDetector.getObjectContours(image);

	std::vector<cv::Point> approx;
	// test each contour
	for( size_t i = 0; i < contours.size(); i++ )
	{
		cv::Point* pnts = (cv::Point*)malloc(sizeof(cv::Point) * contours[i].size());
		for (int j = 0; j < contours[i].size(); j++)
		{
			pnts[j] = contours[i][j];
		}

		const cv::Point* ppt[1] = { pnts };
		int npt[] = { (int)contours[i].size() };
		fillPoly(output, ppt, npt, 1, cv::Scalar(120, 250, 50));
		delete pnts;
	}

	return contours;
}

#ifdef __cplusplus
- (void)processImage:(Mat&)image;
{
	Mat image_copy;
    cvtColor(image, image_copy, CV_BGRA2BGR);

	if(imageForSegmentationHasBeenTaken){
		if(isFirst){
			isFirst = !isFirst;
			firstImage = image_copy;
			track->setReferenceFrame(firstImage);
			cv::Mat temp;
			track->setObjectsToBeModeled(getLCDetection(firstImage, temp));
			
		}
		else {
			track->processFrame(image_copy, image_copy);

		}
		cvtColor(image_copy, image, CV_BGR2BGRA);
	}
	else{

		//image = showWatershedSegmentation(image_copy);
		getLCDetection(image, image);
	}
	
	
}

Mat getWatershedSegmentation(Mat image)
{
    //Mat blank(image.size(),CV_8U,Scalar(0xFF));

    //Create watershed segmentation object
    WatershedSegmenter segmenter;
    segmenter.createMarkersFromImage(image);
	Mat wshedMask = segmenter.findSegments(image);
	Mat mask;
	segmenter.getSegmentedMask(wshedMask, mask);

    return mask;
}

#endif

-(IBAction)resetCameraFirstPositionButton:(id)sender {
	[self.videoCamera stop];
	isFirst = YES;
	[self.videoCamera start];
}

-(IBAction)markObjectButton:(id)sender {
	imageForSegmentationHasBeenTaken = YES;
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
