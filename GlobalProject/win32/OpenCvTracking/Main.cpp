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
#include "opencv2/photo/photo.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Core/ObjectTrackingSample.h"
#include "Core/VideoTracking.hpp"
#include "Core/WatershedSegmenter.h"
#include "Core/LcObjectDetector.h"

//#include "Poly2Tri/poly2tri.h"

//#include "Poly2Tri/sweep/cdt.h"

#include "Core/clipper.hpp"
#include "Core/LcInPaint.h"

#include "Core/UnitsConvertor.h"

#include <ctime>

using namespace std;
using namespace cv;

// forward declerations
int destroyAroundMeGame();

// test function
int inPaint();
int clipping();
int simplify();
int triangulation();

int test()
{
    cv::Mat s1(240, 240, CV_8UC3);
    s1 = cv::Scalar(255);

    imshow("s1", s1);

    cv:Mat s2(240, 240, CV_8UC3);
    s2 = cv::Scalar(0, 0, 255);

    imshow("s2", s2);

    cv::Mat mask(240, 240, CV_8UC1);
    mask = 0;

    cv::Point points[4];
    points[0] = cv::Point(90, 90);
    points[1] = cv::Point(150, 90);
    points[2] = cv::Point(150, 150);
    points[3] = cv::Point(90, 150);

    const cv::Point* ppt[1] = { points };
    int npt[] = { 4 };
    fillPoly(mask, ppt, npt, 1, cv::Scalar(255));

    imshow("mask", mask);

    s2.copyTo(s1, mask);

    imshow("copyTo", s1);
    
    cv::Mat s3(240, 240, CV_8UC3);
    s3 = cv::Scalar(255);

    s3 += s2;

    imshow("+=", s3);

    waitKey();

    return 0;
}
/**
* @function main
* @brief Main function
*/
int main( int argc, char** argv )
{
    /*test();
    return 1;*/

    try
    {
        destroyAroundMeGame();
    }
    catch (cv::Exception& ex)
    {
        std::cout << ex.msg << std::endl;
    }
    catch (std::exception& stdEx)
    {
        std::cout << stdEx.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception" << std::endl;
    }

    // some tests functions
    //return simpleTrack();
    //triangulation();
    //inPaint();
    //clipping();
    //simplify();

    return 0;
}




int destroyAroundMeGame()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {  
        return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);//352);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);//288);

    Mat testFrame;
    while(testFrame.empty())
    {
        cap >> testFrame;
    }

    namedWindow("output", 1);
    namedWindow("input", 2);

    VideoTracking *track = new VideoTracking();
    track->getWorld()->setDebugDrawEnabled(false);
    track->setFeatureType(VideoTracking::FeatureType::ORB);

    setMouseCallback("output", VideoTracking::mouseCallback, track);

    vector<vector<Point> > contours;

    time_t start = time(0);
    long frames = 0;
    LcObjectDetector objDetector;
    bool initialized = false;
    int key = 0;
    bool isFirst = false;

    // here starts the game loop
    for(;;)
    {        
        Mat frame;
        cap >> frame; // get a new frame from camera

        // if not initialize - show object detected for the user to choose (until presses i)
        if (!initialized)
        {
            // temp matrices for processing
            Mat tempFrame, contoursFrame;
            frame.copyTo(tempFrame);
            frame.copyTo(contoursFrame);

            // find the contours
            contours = objDetector.getObjectContours(tempFrame);
          
            // draw each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {          
                Point* pnts = new Point[contours[i].size()];
                for (int j = 0; j < contours[i].size(); j++)
                {
                    pnts[j] = contours[i][j];
                }

                const Point* ppt[1] = { pnts };
                int npt[] = { contours[i].size() };
                fillPoly(contoursFrame, ppt, npt, 1, Scalar(120, 250, 50));

                delete[] pnts;
            }

            // show to the user
            imshow("input", frame);
            imshow("output", contoursFrame);

            //wait 5 ms for key press
            key = waitKey(5);

            // if user clicked "i" - initialize scene
            if (105 == key)
            {
                // mark as initialized
                initialized = true;

                try
                {
                    // set the reference from
                    track->setReferenceFrame(frame);

                    // set the objects according to last found contours
                    track->getWorld()->setObjectsToBeModeled(contours);

                    // prepare in-painted scene for later use
                    track->prepareInPaintedScene(frame, contours);
                }
                catch (Exception ex)
                {
                    std::cout << ex.msg << std::endl;
                }

                //imshow("inpainted", track->m_inpaintedScene);
                //waitKey();
            }
        }
        else
        {     
            try

            {
                Mat image_copy;
                cvtColor(frame, image_copy, CV_BGRA2BGR);

                track->processFrame(image_copy, image_copy);

                cvtColor(image_copy, frame, CV_BGR2BGRA);

                imshow("output", frame);
            }
            catch (Exception ex)
            {
                std::cout << ex.msg << std::endl;
            }
        }

        /* frame rate calculation
        ++frames;
        if (frames % 100 == 0)
        {
            double diff = time(0) - start;
            cout << frames / diff << " fps" << endl;
        }   
        */ 
        if(waitKey(5) >= 0) break;
    }

    return 1;
}


/*
    following are some test functions...
*/
// simple function to make a circle as a path (clipper format)
ClipperLib::Path makeCircle(ClipperLib::IntPoint center, int radius, int numPoints)
{
    ClipperLib::Path path;

    int step = 360 / numPoints;

    for (int i = 0; i < 360; i += step)
    {
        path.push_back(ClipperLib::IntPoint(center.X + cos(i * CV_PI / 180) * radius, center.Y + sin(i * CV_PI / 180) * radius));
    }

    return path;
}

// test the simplify method of object detector
int simplify()
{

    Mat input;

    input.create(300, 300, CV_8UC1);

    input = Scalar(255);

    LcObjectDetector detector;

    
    ClipperLib::Paths paths;
    ClipperLib::Path p1;
    p1.push_back(ClipperLib::IntPoint(100, 100));
    p1.push_back(ClipperLib::IntPoint(200, 100));
    p1.push_back(ClipperLib::IntPoint(200, 200));
    p1.push_back(ClipperLib::IntPoint(100, 200));
    paths.push_back(p1);



    p1.clear();
    p1.push_back(ClipperLib::IntPoint(150, 150));
    p1.push_back(ClipperLib::IntPoint(250, 150));
    p1.push_back(ClipperLib::IntPoint(250, 250));
    p1.push_back(ClipperLib::IntPoint(150, 250));
    paths.push_back(p1);

    p1.clear();
    p1.push_back(ClipperLib::IntPoint(0, 0));
    p1.push_back(ClipperLib::IntPoint(90, 0));
    p1.push_back(ClipperLib::IntPoint(90, 90));
    p1.push_back(ClipperLib::IntPoint(0, 90));
    paths.push_back(p1);

    p1.clear();
    p1.push_back(ClipperLib::IntPoint(20, 20));
    p1.push_back(ClipperLib::IntPoint(80, 20));
    p1.push_back(ClipperLib::IntPoint(80, 80));
    p1.push_back(ClipperLib::IntPoint(20, 80));
    paths.push_back(p1);

    p1.clear();
    p1.push_back(ClipperLib::IntPoint(112, 175));
    p1.push_back(ClipperLib::IntPoint(75, 212));
    p1.push_back(ClipperLib::IntPoint(112, 250));
    paths.push_back(p1);

    paths.push_back(makeCircle(ClipperLib::IntPoint(180, 120), 50, 12));

    for (int i = 0; i < paths.size(); i++)
    {
        for (int j = 0; j < paths[i].size() - 1; j++)
        {
            line(input, UnitsConvertor::ClipperPointToCvPoint(paths[i][j]), UnitsConvertor::ClipperPointToCvPoint(paths[i][j + 1]), cv::Scalar(0));
        }
        line(input, UnitsConvertor::ClipperPointToCvPoint(paths[i][paths[i].size() - 1]), UnitsConvertor::ClipperPointToCvPoint(paths[i][0]), cv::Scalar(0));
    }

    imshow("input", input);

    ClipperLib::Paths simple = detector.simplify(paths);

    cout << simple.size() << " after simplyfication" << endl;

    input = Scalar(255);
    for (int i = 0; i < simple.size(); i++)
    {
        for (int j = 0; j < simple[i].size() - 1; j++)
        {
            line(input, UnitsConvertor::ClipperPointToCvPoint(simple[i][j]), UnitsConvertor::ClipperPointToCvPoint(simple[i][j + 1]), cv::Scalar(0));
        }
        line(input, UnitsConvertor::ClipperPointToCvPoint(simple[i][simple[i].size() - 1]), UnitsConvertor::ClipperPointToCvPoint(simple[i][0]), cv::Scalar(0));
    }

    imshow("output", input);

    waitKey();

    return 0;
}

int clipping()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {
        return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 352);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 288);

    Mat testFrame;
    while(testFrame.empty())
    {
        cap >> testFrame;
    }

    namedWindow("output", 1);
    namedWindow("input", 2);

    vector<vector<Point> > contours;

    time_t start = time(0);
    long frames = 0;
    LcObjectDetector objDetector;
    bool initialized = false;
    int key = 0;

    Mat frame;    
    cap >> frame; // get a new frame from camera


    //draw
    cv::Scalar blue(200, 45, 60);
    line(frame, cv::Point(100, 100), cv::Point(200, 100), blue, 2);
    line(frame, cv::Point(200, 100), cv::Point(200, 200), blue, 2);
    //line(frame, cv::Point(200, 200), cv::Point(100, 200), blue, 2);
    line(frame, cv::Point(200, 200), cv::Point(100, 100), blue, 2);

    cv::Scalar red(38, 45, 200);
    line(frame, cv::Point(80, 80), cv::Point(120, 80), red, 2);
    line(frame, cv::Point(120, 80), cv::Point(120, 120), red, 2);
    line(frame, cv::Point(120, 120), cv::Point(80, 120), red, 2);
    line(frame, cv::Point(80, 120), cv::Point(80, 80), red, 2);

    imshow("input", frame);
    
    // define object
    ClipperLib::Paths object(1);
    object[0].push_back(ClipperLib::IntPoint(100, 100));
    object[0].push_back(ClipperLib::IntPoint(200, 100));
    object[0].push_back(ClipperLib::IntPoint(100, 200));

    // define clipping
    ClipperLib::Paths clip(1);
    clip[0].push_back(ClipperLib::IntPoint(80, 80));
    clip[0].push_back(ClipperLib::IntPoint(120, 80));
    clip[0].push_back(ClipperLib::IntPoint(120, 120));
    clip[0].push_back(ClipperLib::IntPoint(80, 120));

    // do the clipping and get the solution
    ClipperLib::Clipper clipper;

    ClipperLib::Paths solution;
    clipper.AddPaths(object, ClipperLib::ptSubject, true);
    clipper.AddPaths(clip, ClipperLib::ptClip, true);
    
    cv::Scalar orange(0, 100, 255);
    if(clipper.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd))
    {
        // draw the solution(s)
        for (int i = 0; i < solution.size(); i++)
        {
            for (int j = 0; j < solution[i].size() - 1; j++)
            {
                line(frame, cv::Point(solution[i][j].X, solution[i][j].Y), cv::Point(solution[i][j + 1].X, solution[i][j + 1].Y), orange, 1);
            }

            line(frame, cv::Point(solution[i][solution[i].size() - 1].X, solution[i][solution[i].size() - 1].Y), cv::Point(solution[i][0].X, solution[i][0].Y), orange, 1);
        }
    }      

    imshow("output", frame);

    key = waitKey();      
}

int inPaint()
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {
         return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 352);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 288);

    Mat testFrame;
    while(testFrame.empty())
    {
        cap >> testFrame;
    }

    
    bool initialized = false;
    LcObjectDetector objDetector;
    std::vector<std::vector<cv::Point>> contours;
    int key = 0;

    Mat mask, frame, output;

    for(;;)
    {        
        Mat frame2, frame3;

        cap >> frame; // get a new frame from camera

        frame.copyTo(frame2);
        frame.copyTo(frame3);

        contours = objDetector.getObjectContours(frame2);

        vector<Point> approx;
        // test each contour
        for( size_t i = 0; i < contours.size(); i++ )
        {          
            Point* pnts = new Point[contours[i].size()]; //(Point*)malloc(sizeof(Point) * contours[i].size());
            for (int j = 0; j < contours[i].size(); j++)
            {
                pnts[j] = contours[i][j];
            }

            const Point* ppt[1] = { pnts };
            int npt[] = { contours[i].size() };
            fillPoly(frame3, ppt, npt, 1, Scalar(120, 250, 50));

            delete[] pnts;
        }

        imshow("input", frame);
        imshow("edges", frame3);

        key = waitKey(30);

        if (105 == key)
        {
            break;
        }
        
    }

    LcInPaint inpaint;
    inpaint.inpaint(frame, contours, output);

    //cv::inpaint(frame, mask, output, 15, INPAINT_TELEA);

    imshow("results", output);
    
    waitKey();
}

int triangulation()
{
    /*
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

        imshow("input", frame);        

        std::vector<p2t::Point*> polygon;

        polygon.push_back(new p2t::Point(100,100));
        polygon.push_back(new p2t::Point(200,100));
        polygon.push_back(new p2t::Point(200,200));
        polygon.push_back(new p2t::Point(100,200));

        p2t::CDT* cdt = new p2t::CDT(polygon);

        cdt->Triangulate();

        std::vector<p2t::Triangle*> triangles = cdt->GetTriangles();

        cv:Scalar color(200, 100, 50);
        for (int idx = 0; idx < triangles.size(); idx++)
        {
            p2t::Triangle* triangle = triangles[idx];

            cv::Point p1(triangle->GetPoint(0)->x, triangle->GetPoint(0)->y);
            cv::Point p2(triangle->GetPoint(1)->x, triangle->GetPoint(1)->y);
            cv::Point p3(triangle->GetPoint(2)->x, triangle->GetPoint(2)->y);

            line(frame, p1, p2, color, 2);
            line(frame, p2, p3, color, 2);
            line(frame, p3, p1, color, 2);

            // Find the mid-point of the triangle
            cv::Point midPoint((p1.x + p2.x + p1.x) / 3, (p1.y + p2.y + p1.y) / 3);

            // draw the lines
            line(frame, midPoint, p1, color, 2);
            line(frame, midPoint, p2, color, 2);
            line(frame, midPoint, p3, color, 2);
        }


        imshow("output", frame);

        key = waitKey(30);

        if (key > 0)
        {
            break;
        }
    }    
    */
    return 1;
}

