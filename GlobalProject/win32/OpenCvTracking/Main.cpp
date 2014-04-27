/**
 * @file SURF_Homography
 * @brief SURF detector + descriptor + FLANN Matcher + FindHomography
 * @author A. Huaman
 */

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
//#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include "Core/ObjectTrackingSample.h"
#include "Core/VideoTracking.hpp"
#include "Core/WatershedSegmenter.h"
#include "Core/LcObjectDetector.h";

#include <ctime>

using namespace std;
using namespace cv;

// forward declerations
void readme();
int simpleTrack();

void processImage(Mat& image);
int track2();

bool isFirst = true;
Mat firstImage;
//ObjectTrackingSample *track;
VideoTracking *track;

/**
 * @function main
 * @brief Main function
 */
int main( int argc, char** argv )
{
    //return simpleTrack();
    track2();
}

Mat showWatershedSegmentation(Mat image)
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


int track2()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
 
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 352);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 288);

    Mat testFrame;
    while(testFrame.empty())
    {
        cap >> testFrame;
    }

    namedWindow("output", 1);
    namedWindow("input", 2);

    track = new VideoTracking();

    setMouseCallback("output", VideoTracking::mouseCallback, track );

    vector<vector<Point> > contours;

    time_t start = time(0);
    long frames = 0;
    LcObjectDetector objDetector;
    bool initialized = false;
    int key = 0;

    for(;;)
    {        
        Mat frame, frame2, frame3;

        
        cap >> frame; // get a new frame from camera

        
        if (!initialized)
        {
            frame.copyTo(frame2);
            frame.copyTo(frame3);

        
            contours = objDetector.getObjectContours(frame2);

            vector<Point> approx;
            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {          
                Point* pnts = (Point*)malloc(sizeof(Point) * contours[i].size());
                for (int j = 0; j < contours[i].size(); j++)
                {
                    pnts[j] = contours[i][j];
                }

                const Point* ppt[1] = { pnts };
                int npt[] = { contours[i].size() };
                fillPoly(frame3, ppt, npt, 1, Scalar(120, 250, 50));

                delete pnts;
            }

            imshow("input", frame);
            imshow("output", frame3);

            key = waitKey(30);

            if (105 == key)
            {
                initialized = true;
                isFirst = false;
                
                track->setReferenceFrame(frame);
                track->setObjectsToBeModeled(contours);
            }
        }
        else
        {        
            processImage(frame);
            imshow("output", frame);
            if(waitKey(30) >= 0) break;
        }

/*        ++frames;
        if (frames % 100 == 0)
        {
            double diff = time(0) - start;
            cout << frames / diff << " fps" << endl;
        }   */        
    }

    return 1;
}


    // this function is being called for each frame
void processImage(Mat& image)
{
    Mat image_copy;
    cvtColor(image, image_copy, CV_BGRA2BGR);

    if(isFirst){
        // if this is the first image, we need to save a reference frame to compare to
        cvtColor(image, firstImage, CV_BGRA2BGR);     
        track->setReferenceFrame(firstImage);
        isFirst = !isFirst;
    }
    else {
        // if this is not the first time, 
        //then 'track' is initialized and has a frame to compare to so we can process the current frame
        track->processFrame(image_copy, image_copy);
    }

    cvtColor(image_copy, image, CV_BGR2BGRA);
}

int simpleTrack()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    
    namedWindow("edges",1);


    FlannBasedMatcher* matcher_ = new FlannBasedMatcher(new cv::flann::LshIndexParams(5, 24, 2));

    FlannBasedMatcher matcher;
    vector< DMatch > matches;

    OrbFeatureDetector detector;
    OrbDescriptorExtractor extractor;

    //SurfFeatureDetector detector;
    //SurfDescriptorExtractor extractor;

    Mat img;
    Mat img_prev;
   
    vector<KeyPoint> keypoints_prev;
    vector<KeyPoint> keypoints_object;

    Mat descriptors_object;
    Mat descriptors_prev;
 
    int itr = 0;
    for(;;)
    {        
        Mat frame;    // define a matrix for frame
        cap >> frame; // get a new frame from camera
        cvtColor(frame, img, CV_BGR2GRAY); // convert to grayscale

        // detect features and extract descriptors for current image
        
        detector.detect( img, keypoints_object );
        extractor.compute( img, keypoints_object, descriptors_object );

        // if we have data from previous image
        if (descriptors_prev.data && itr++ % 1000 == 1)
        {
            try
            {
                FlannBasedMatcher matcher;
                std::vector< DMatch > matches;
                matcher_->match( descriptors_object, descriptors_prev, matches );

                double max_dist = 0; double min_dist = 100;

                //-- Quick calculation of max and min distances between keypoints
                for( int i = 0; i < matches.size(); i++ )
                { 
                    double dist = matches[i].distance;
                    if( dist < min_dist ) min_dist = dist;
                    if( dist > max_dist ) max_dist = dist;
                }

                cout << "-- Max dist : " << max_dist << endl;
                cout << "-- Min dist : " << min_dist << endl;

                //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
                std::vector< DMatch > good_matches;

                for( int i = 0; i < matches.size(); i++ )
                { 
                    if( matches[i].distance < 3*min_dist ) { 
                        good_matches.push_back( matches[i]); 
                    }
                }

                Mat img_matches;
                drawMatches( img, keypoints_object, img_prev, keypoints_prev,
                            good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                            vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

                //-- Localize the object
                std::vector<Point2f> obj;
                std::vector<Point2f> scene;

                for( int i = 0; i < good_matches.size(); i++ )
                {
                    //-- Get the keypoints from the good matches
                    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
                    scene.push_back( keypoints_prev[ good_matches[i].trainIdx ].pt );
                }

                try
                {
                    if (obj.size() > 3 && scene.size() > 3)
                    {
                        Mat H = findHomography( obj, scene, CV_RANSAC );

                        //-- Get the corners from the image_1 ( the object to be "detected" )
                        std::vector<Point2f> obj_corners(4);
                        obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img.cols, 0 );
                        obj_corners[2] = cvPoint( img.cols, img.rows ); obj_corners[3] = cvPoint( 0, img.rows );
                        std::vector<Point2f> scene_corners(4);

                        perspectiveTransform( obj_corners, scene_corners, H);

                        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
                        line( img_matches, scene_corners[0] + Point2f( img.cols, 0), scene_corners[1] + Point2f( img.cols, 0), Scalar(0, 255, 0), 4 );
                        line( img_matches, scene_corners[1] + Point2f( img.cols, 0), scene_corners[2] + Point2f( img.cols, 0), Scalar( 0, 255, 0), 4 );
                        line( img_matches, scene_corners[2] + Point2f( img.cols, 0), scene_corners[3] + Point2f( img.cols, 0), Scalar( 0, 255, 0), 4 );
                        line( img_matches, scene_corners[3] + Point2f( img.cols, 0), scene_corners[0] + Point2f( img.cols, 0), Scalar( 0, 255, 0), 4 );

                        //-- Show detected matches
                        imshow( "Good Matches & Object detection", img_matches );
                    }
                } catch (Exception ex) {
                    cout << "Exception in homography block" << ex.msg << endl;
                }

                //std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            //catch (cv::Exception ex)
            //{
            //    cout << "CV Exception " << ex.msg;
            //}
            catch (Exception ex)
            {
                cout << ex.msg;
            }

        }

        //save current as old
        img_prev = img;
        keypoints_prev = keypoints_object;
        descriptors_prev = descriptors_object;
      
        imshow("edges", img);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

int matchImages(int argc, char** argv)
{
  if( argc != 3 )
  { readme(); return -1; }

  Mat img_object = imread( argv[1], IMREAD_GRAYSCALE );
  Mat img_scene = imread( argv[2], IMREAD_GRAYSCALE );

  if( !img_object.data || !img_scene.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 200;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_object, keypoints_scene;

  detector.detect( img_object, keypoints_object );
  detector.detect( img_scene, keypoints_scene );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_object, descriptors_scene;

  extractor.compute( img_object, keypoints_object, descriptors_object );
  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_object, descriptors_scene, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_object.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance < 3*min_dist )
    { good_matches.push_back( matches[i]); }
  }

  Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );


  //-- Localize the object from img_1 in img_2
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( size_t i = 0; i < good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
  }


  Mat H = findHomography( obj, scene, RANSAC );

  //-- Get the corners from the image_1 ( the object to be "detected" )
  std::vector<Point2f> obj_corners(4);
  obj_corners[0] = Point(0,0); obj_corners[1] = Point( img_object.cols, 0 );
  obj_corners[2] = Point( img_object.cols, img_object.rows ); obj_corners[3] = Point( 0, img_object.rows );
  std::vector<Point2f> scene_corners(4);

  perspectiveTransform( obj_corners, scene_corners, H);


  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
  Point2f offset( (float)img_object.cols, 0);
  line( img_matches, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 4 );
  line( img_matches, scene_corners[1] + offset, scene_corners[2] + offset, Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[2] + offset, scene_corners[3] + offset, Scalar( 0, 255, 0), 4 );
  line( img_matches, scene_corners[3] + offset, scene_corners[0] + offset, Scalar( 0, 255, 0), 4 );

  //-- Show detected matches
  imshow( "Good Matches & Object detection", img_matches );

  waitKey(0);

  return 0;
}

/**
 * @function readme
 */
void readme()
{ 
    std::cout << " Usage: ./SURF_Homography <img1> <img2>" << std::endl; 
}