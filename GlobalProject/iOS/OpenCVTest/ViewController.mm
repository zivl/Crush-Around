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
@synthesize fontLarge;
@synthesize fontSmall;
@synthesize blowItUpLabel;
@synthesize blowItUpPanel;
@synthesize scoreLabel;
@synthesize scoringPanel;
@synthesize innerScorePanelBG;
@synthesize scorePoints;
@synthesize score;
- (void)setScore:(NSInteger)iScore {
    score = iScore;
    [self.scorePoints setText:[NSNumberFormatter localizedStringFromNumber:@(score) numberStyle:NSNumberFormatterDecimalStyle]];
}

BOOL isFirst = YES;
BOOL imageForSegmentationHasBeenTaken = NO;

Mat firstImage;
VideoTracking *track;
std::vector<cv::Point> touchPoints;

- (void)viewDidLoad
{
	[super viewDidLoad];

	[self loadGameControls];

	[self configureImageCameraAndImageProcessingObjects];
	[self configureGestures];

}

-(void)configureImageCameraAndImageProcessingObjects{
    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView];
	self.videoCamera.delegate = self;
    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    self.videoCamera.defaultFPS = 30;
    self.videoCamera.grayscaleMode = NO;
	[self.videoCamera start];
}

-(void)configureGestures {
	/*
	UITapGestureRecognizer *gestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
	[imageView addGestureRecognizer:gestureRecognizer];
	gestureRecognizer.delegate = self;
	 */

	UIPanGestureRecognizer *gestureRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onFingerPan:)];
	[imageView addGestureRecognizer:gestureRecognizer];
	gestureRecognizer.delegate = self;
}

static const CGFloat kCornerRadius = 10.0f;

-(void)loadGameControls {
	self.fontLarge = [UIFont fontWithName:@"GROBOLD" size:18.0f];
    self.fontSmall = [UIFont fontWithName:@"GROBOLD" size:14.0f];
	[self.blowItUpLabel setFont:self.fontLarge];
	[self.innerScorePanelBG.layer setCornerRadius:kCornerRadius];
    [self.innerScorePanelBG setClipsToBounds:YES];
    [self.innerScorePanelBG setBackgroundColor:[UIColor clearColor]];
    [self.scorePoints setFont:self.fontLarge];
    [self.scoreLabel setFont:self.fontLarge];
	[self setScore:0];
}

#pragma mark - Protocol CvVideoCameraDelegate
#ifdef __cplusplus
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


std::vector<std::vector<cv::Point>> contours;

- (void)processImage:(Mat&)image;
{
	Mat image_copy;
    cvtColor(image, image_copy, CV_BGRA2BGR);

	if(imageForSegmentationHasBeenTaken){
		if(isFirst){
			isFirst = !isFirst;
			firstImage = image_copy;
			track = new VideoTracking();
			track->setDebugDraw(true);
			track->setReferenceFrame(firstImage);
			track->setObjectsToBeModeled(contours);
			track->prapreInPaintedScene(image_copy, contours);
		}
		else {
			track->processFrame(image_copy, image_copy);
		}
		cvtColor(image_copy, image, CV_BGR2BGRA);
	}
	else{
		//image = showWatershedSegmentation(image_copy);
		contours = getLCDetection(image, image);
	}
}

Mat getWatershedSegmentation(Mat image)
{
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

-(IBAction)onBlowItUpButton:(id)sender {
	imageForSegmentationHasBeenTaken = YES;
}

-(void)onTap:(UITapGestureRecognizer *)recognizer {
	if(track){
		CGPoint location = [recognizer locationInView:imageView];
		track->onMouse(1, location.x, location.y, nil, nil);
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	[super touchesBegan:touches withEvent:event];
	NSLog(@"panning began");
	touchPoints.clear();
}

-(void)onFingerPan:(UIPanGestureRecognizer *)recognizer {
	if(recognizer.state == UIGestureRecognizerStateEnded){
		NSLog(@"panning UIGestureRecognizerStateEnded");
		if(track){
			track->onPanGestureEnded(touchPoints);
		}
	}
	else {
		CGPoint location = [recognizer locationInView:imageView];
		NSLog(@"location: [%f, %f]", location.x, location.y);
		touchPoints.push_back(cv::Point(location.x, location.y));
	}
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
