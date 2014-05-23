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
#import "SpriteLayer.h"
#import "GameConfiguration.h"


@interface ViewController ()


@end

@implementation ViewController
@synthesize videoCamera;
@synthesize fontLarge;
@synthesize fontSmall;
@synthesize notificationView;
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

@synthesize timerPanelBG;
@synthesize timerPanelView;
@synthesize timerTimeLeftLabel;
@synthesize timerTitleLabel;
@synthesize timeInSeconds;
#define kMAX_TIME_SECONDS 3600
#define kMIN_TIME_SECONDS -1
-(void)setTimeInSeconds:(int)iTimeInSeconds{
	if(kMIN_TIME_SECONDS < iTimeInSeconds && iTimeInSeconds < kMAX_TIME_SECONDS){
		timeInSeconds = iTimeInSeconds;
	}
	else {
		NSLog(@"invalid time was set: %d", iTimeInSeconds);
	}
	[self updateCountdown];
}


BOOL isFirst = YES;
BOOL imageForSegmentationHasBeenTaken = NO;
NSTimer *mainGameTimer;
UIActivityIndicatorView *activityView;
Mat firstImage;
VideoTracking *track;
std::vector<cv::Point> touchPoints;

#define CAMERA_VIEW_CENTER CGPointMake(imageView.bounds.size.width / 2, imageView.bounds.size.height / 2)


- (void)viewDidLoad
{
	[super viewDidLoad];

	[self loadGameControls];
//	[self.notificationView showNotificationWithMessage:@"Find an object to blow up :)"];
	[self configureImageCameraAndImageProcessingObjects];
}


-(void)configureImageCameraAndImageProcessingObjects{
    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView];
	self.videoCamera.delegate = self;
    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPresetMedium;
    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeRight;
	self.videoCamera.rotateVideo = YES;
    self.videoCamera.defaultFPS = 30;
    self.videoCamera.grayscaleMode = NO;
	[self.videoCamera start];
}

-(void)configureGestures {
	UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onFingerPan:)];
	[imageView addGestureRecognizer:panGestureRecognizer];
	panGestureRecognizer.delegate = self;

	UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onFingerTap:)];
	[imageView addGestureRecognizer:tapGestureRecognizer];
	tapGestureRecognizer.delegate = self;
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

	[self.timerPanelBG.layer setCornerRadius:kCornerRadius];
    [self.timerPanelBG setClipsToBounds:YES];
    [self.timerPanelBG setBackgroundColor:[UIColor clearColor]];
	[self.timerTimeLeftLabel setFont:self.fontLarge];
	[self.timerTitleLabel setFont:self.fontLarge];

	[self setScore:0];
	[self updateCountdown];

	[self createNotificationView];

}

-(void)createNotificationView{
	self.notificationView = [[NotificationView alloc] init];
	[self.notificationView.message setFont:self.fontSmall];
	[self.notificationView.okButton.titleLabel setFont:self.fontLarge];
	[self.notificationView.bg.layer setCornerRadius:kCornerRadius];
    [self.notificationView.bg setClipsToBounds:YES];
    [self.notificationView.bg setBackgroundColor:[UIColor clearColor]];

	[imageView addSubview:self.notificationView];

	self.notificationView.center = CAMERA_VIEW_CENTER;
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
		for (int j = 0; j < contours[i].size(); j++){
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
			firstImage = image_copy;
			track = new VideoTracking();
//			track->setFeatureType(VideoTracking::FeatureType::SIFT);
			[self assignListenersToVideoTracker];
			track->getWorld()->setDebugDrawEnabled(false);
			track->setRestrictBallInScene(true);
			track->setReferenceFrame(firstImage);
			track->getWorld()->setObjectsToBeModeled(contours);
			track->prepareInPaintedScene(image_copy, contours);
			int numberOfObjects = (int)contours.size();
			double area = 0.0;
			for(int i = 0; i < numberOfObjects; i++){
				area += std::abs(cv::contourArea(cv::Mat(contours[i])));
			}
			[self calculateNecessaryTimeForArea: area andNumberOfObjects: numberOfObjects];
			isFirst = !isFirst;
		}
		else {
			track->processFrame(image_copy, image_copy);
		}
		cvtColor(image_copy, image, CV_BGR2BGRA);
	}
	else{
		contours = getLCDetection(image, image);
	}
}

-(void) assignListenersToVideoTracker {
	track->delegateBallHitObserver((^(float x, float y) {
		LCPoint *point = [[LCPoint alloc] init];
		point.x = x;
		point.y = y;
		[self performSelectorOnMainThread:@selector(ballHitAtPoint:) withObject:point waitUntilDone:NO];
	}));
	track->attachObjectsDestryedObserver((^(){
		[self performSelectorOnMainThread:@selector(onAllObjectsHaveBeenDestroyed) withObject:nil waitUntilDone:NO];
	}));
	track->attachBallInSceneObserver((^(){
		[self performSelectorOnMainThread:@selector(onBallNotInScene) withObject:nil waitUntilDone:NO];
	}));
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

-(void)onBallNotInScene {
	[self wrapUpAndFinishTheGame:GameEndedDueToBallWentOutOfScene];
}

-(void)onAllObjectsHaveBeenDestroyed {
	[self wrapUpAndFinishTheGame:GameEndedDueToAllObjectsHasBeenDestroyed];
}

-(void)ballHitAtPoint:(LCPoint *) point {
	[self showExplosionAtPoint: [point getCGPoint]];
	[self setScore:self.score + 12];
}

-(void)calculateNecessaryTimeForArea:(double)area andNumberOfObjects:(int) numberOfObjects{
	int time = 60;//area / 10 / numberOfObjects / 2;
	self.timeInSeconds = time;
}

-(void)startTimer{
	mainGameTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(timerFireMethod:) userInfo:self repeats:YES];
}

- (void)timerFireMethod:(NSTimer *)timer {
	if(!isFirst){
		if(activityView.isAnimating){
			[activityView stopAnimating];
			[activityView removeFromSuperview];
		}
		self.timeInSeconds--;
		if(self.timeInSeconds == 0){
			[self wrapUpAndFinishTheGame: GameEndedDueToTimesUp];
		}
	}
}

-(void)wrapUpAndFinishTheGame:(GameEnded)reason {
	[mainGameTimer invalidate];
	[self.videoCamera stop];
	self.videoCamera = nil;
	imageView.opaque = NO;
	UIImageView *bg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"home_bg"]];
	[self.view insertSubview:bg belowSubview:imageView];

	[[NSUserDefaults standardUserDefaults] setInteger:self.score forKey:SCORE_KEY];
	NSString *finalMsg;
	NSString *totalScore = [NSString stringWithFormat:@"Total Score: %ld", (long)self.score];
	switch (reason) {
		case GameEndedDueToTimesUp:
			finalMsg = [NSString stringWithFormat:@"Your time is up! %@", totalScore];
			break;
		case GameEndedDueToAllObjectsHasBeenDestroyed:
			finalMsg = [NSString stringWithFormat:@"All Objects Have Been Destroyed! %@", totalScore];
			break;
		case GameEndedDueToBallWentOutOfScene:
			finalMsg = [NSString stringWithFormat:@"Ball is no longer in the scene! %@", totalScore];
			break;
		default:
			finalMsg = [NSString stringWithFormat:@"Game Over! %@", totalScore];
			break;
	}
	[self.notificationView showNotificationWithMessage:finalMsg withTarget:self withAction:@selector(onGameOverOKButton)];
}


-(void)onGameOverOKButton {
	[self.notificationView removeHandlerWithTarget:self withAction:@selector(onGameOverOKButton)];
	[self dismissViewControllerAnimated:YES completion:nil];
}

-(void) updateCountdown {
    int minutes = (self.timeInSeconds % 3600) / 60;
	int seconds = (self.timeInSeconds % 3600) % 60;
    self.timerTimeLeftLabel.text = [NSString stringWithFormat:@"%02d:%02d", minutes, seconds];
}

-(IBAction)onBlowItUpButton:(id)sender {
	if(!imageForSegmentationHasBeenTaken){
		imageForSegmentationHasBeenTaken = YES;
		activityView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		activityView.hidesWhenStopped = YES;
		[self.view addSubview:activityView];
		activityView.center = CAMERA_VIEW_CENTER;
		[activityView startAnimating];
		[UIView animateWithDuration:0.5 animations:^{
			self.blowItUpPanel.alpha = 0.0;
		} completion:^(BOOL finished) {
			[self.blowItUpPanel removeFromSuperview];
		}];
		[self configureGestures];
		[self startTimer];
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	[super touchesBegan:touches withEvent:event];
	touchPoints.clear();
}

-(void)onFingerTap:(UITapGestureRecognizer *)recognizer {
	CGPoint location = [recognizer locationInView:imageView];
	track->onMouse(1, location.x, location.y, nil, nil);
	[self reduceScore: 1];
}

-(void)onFingerPan:(UIPanGestureRecognizer *)recognizer {
	if(recognizer.state == UIGestureRecognizerStateEnded){
		if(track){
			track->onPanGestureEnded(touchPoints);
			int numberOfPointsToReduce = (int)touchPoints.size();
			[self reduceScore:numberOfPointsToReduce];
		}
	}
	else {
		CGPoint location = [recognizer locationInView:imageView];
		touchPoints.push_back(cv::Point(location.x, location.y));
	}
}

-(void)reduceScore:(int) points {
	[self setScore: self.score - points];
}

- (void)showExplosionAtPoint:(CGPoint)point {
    // (1) Create the explosion sprite
    UIImage * explosionImageOrig = [UIImage imageNamed:@"explosion"];
    CGImageRef explosionImageCopy = CGImageCreateCopy(explosionImageOrig.CGImage);
    CGSize explosionSize = CGSizeMake(128, 128);
    SpriteLayer * sprite = [SpriteLayer layerWithImage:explosionImageCopy spriteSize:explosionSize];
    CFRelease(explosionImageCopy);

    // (2) Position the explosion sprite
    CGFloat xOffset = 50.0f;
	CGFloat yOffset = 20.0f;
    sprite.position = CGPointMake(point.x + xOffset, point.y + yOffset);

    // (3) Add to the view
    [self.view.layer addSublayer:sprite];

    // (4) Configure and run the animation
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"spriteIndex"];
    animation.fromValue = @(1);
    animation.toValue = @(12);
    animation.duration = 0.45f;
    animation.repeatCount = 1;
    animation.delegate = sprite;

    [sprite addAnimation:animation forKey:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
