#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "Definitions.h"
#include "WatershedSegmenter.h"
#include "GrabCutSegmenter.h"


using namespace cv;
using namespace std;

Mat showBlobs(Mat image, bool drawLinesBetweenBlobs){
        Mat out;
        vector<KeyPoint> keyPoints(1024);
        SimpleBlobDetector::Params params;
		params.filterByColor = false;
        params.filterByCircularity = false;

		params.minDistBetweenBlobs = BLOB_DISTANCE_BETWEEEN_BLOBS;
        params.minThreshold = BLOB_MIN_THRESHOLD;
        params.maxThreshold = BLOB_MAX_THRESHOLD;
        params.thresholdStep = BLOB_THRESHOLD_STEP;

        params.minArea = BLOB_MIN_AREA; 
        params.maxArea = BLOB_MAX_AREA;
                
		params.minConvexity = BLOB_MIN_CONVEXITY;
        params.maxConvexity = BLOB_MAX_CONVEXITY;

		params.minInertiaRatio = BLOB_MIN_INERTIA_RATIO;

        SimpleBlobDetector blobDetector(params);
		blobDetector.create("SimpleBlob");

        blobDetector.detect(image, keyPoints);
        drawKeypoints(image, keyPoints, out, CV_RGB(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		if(drawLinesBetweenBlobs){
			for(int i = 0; i < keyPoints.size(); i++) {
				if(i + 1 < keyPoints.size()){
					line(out, keyPoints[i].pt, keyPoints[i + 1].pt, CV_RGB(0, 255, 0), 2);
				}
				else {
					line(out, keyPoints[i].pt, keyPoints[0].pt, CV_RGB(0, 255, 0), 2);
				}
			}
		}

        cout << "Keypoints " << keyPoints.size() << endl;
        
        return out;
}

#define SEG_OUTER_BG 10

Mat showWatershedSegmentation(Mat image)
{
    Mat blank(image.size(),CV_8U,Scalar(0xFF));
    Mat dest;
    imshow("originalimage", image);

    // Create markers image
    Mat markers(image.size(),CV_8U,Scalar(-1));

    //top rectangle
    markers(Rect(0,0,image.cols, SEG_OUTER_BG)) = Scalar::all(1);
    //bottom rectangle
    markers(Rect(0,image.rows-SEG_OUTER_BG,image.cols, SEG_OUTER_BG)) = Scalar::all(1);
    //left rectangle
    markers(Rect(0,0,SEG_OUTER_BG,image.rows)) = Scalar::all(1);
    //right rectangle
    markers(Rect(image.cols-SEG_OUTER_BG,0,SEG_OUTER_BG,image.rows)) = Scalar::all(1);
    //centre rectangle
    int centreW = image.cols/4;
    int centreH = image.rows/4;
    markers(Rect((image.cols/2)-(centreW/2),(image.rows/2)-(centreH/2), centreW, centreH)) = Scalar::all(2);
    markers.convertTo(markers,CV_BGR2GRAY);
    imshow("markers", markers);

    //Create watershed segmentation object
    WatershedSegmenter segmenter;
    segmenter.setMarkers(markers);
	Mat wshedMask = segmenter.findSegments(image);
    Mat mask;
    convertScaleAbs(wshedMask, mask, 1, 0);
    double thresh = threshold(mask, mask, 1, 255, THRESH_BINARY);
    bitwise_and(image, image, dest, mask);
    dest.convertTo(dest,CV_8U);

    imshow("final_result", dest);
    waitKey(0);

    return dest;
}

void grabCutSegmentationInteractiveHelp()
{
    cout << "\nThis program demonstrates GrabCut segmentation -- select an object in a region\n"
            "and then grabcut will attempt to segment it out.\n"
            "Call:\n"
            "./grabcut <image_name>\n"
        "\nSelect a rectangular area around the object you want to segment\n" <<
        "\nHot keys: \n"
        "\tESC - quit the program\n"
        "\tr - restore the original image\n"
        "\tn - next iteration\n"
        "\n"
        "\tleft mouse button - set rectangle\n"
        "\n"
        "\tCTRL+left mouse button - set GrabCut Background pixels\n"
        "\tSHIFT+left mouse button - set GrabCut Foreground pixels\n"
        "\n"
        "\tCTRL+right mouse button - set GrabCut Probably Background pixels\n"
        "\tSHIFT+right mouse button - set GrabCut Probably Foreground pixels\n" << endl;
}

GrabCutSegmenter gcapp;

static void on_mouse( int event, int x, int y, int flags, void* param )
{
    gcapp.mouseClick( event, x, y, flags, param );
}

void showGrabCutSegmentation(Mat image){
	const string winName = "image";

	grabCutSegmentationInteractiveHelp();
    namedWindow( winName, WINDOW_AUTOSIZE );
    setMouseCallback( winName, on_mouse, 0 );

    gcapp.setImageAndWinName( image, winName );
    gcapp.showImage();

	bool exitFlagIsOff = true;

    while(exitFlagIsOff)
    {
        int c = waitKey(0);
        switch( (char) c )
        {
        case '\x1b':
            cout << "Exiting ..." << endl;
            exitFlagIsOff = false;
			break;
        case 'r':
            cout << endl;
            gcapp.reset();
            gcapp.showImage();
            break;
        case 'n':
            int iterCount = gcapp.getIterCount();
            cout << "<" << iterCount << "... ";
			gcapp.nextIter();
			int newIterCount = gcapp.getIterCount();
			if(newIterCount > iterCount) {
                gcapp.showImage();
                cout << iterCount << ">" << endl;
            }
            else{
                cout << "rect must be determined>" << endl;
			}
            break;
        }
    }

    destroyWindow(winName);
}
int main( int argc, char** argv )
{
	char filename[MAX_FOLDER_PATH_CHARS] = "";
	
	strcat_s(filename, MAX_FOLDER_PATH_CHARS, INPUT_FOLDER);
	strcat_s(filename, MAX_FOLDER_PATH_CHARS, IMG_ON_WALL_SPEAKERS);

    Mat inputImage = imread(filename);
    if (inputImage.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;	
    }

	showGrabCutSegmentation(inputImage);
	/*Mat outputImage = showWatershedSegmentation(inputImage);

    imshow("Image", outputImage);
    waitKey(0);*/
    
    return 0;
}
