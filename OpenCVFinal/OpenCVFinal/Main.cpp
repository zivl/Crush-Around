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

using namespace cv;
using namespace std;

#define MAX_FOLDER_PATH_CHARS 200
#define INPUT_FOLDER "C:\\vs2012_projects\\Inputs\\"
#define IMG_ON_WALL_SPEAKERS "on-wall-speakers.jpg"
#define IMG_PLANE "plane.jpg"
#define IMG_CARD "card.jpg"

#define BLOB_DISTANCE_BETWEEEN_BLOBS 10.0f
#define BLOB_MIN_AREA 20.0f
#define BLOB_MAX_AREA 500.0f

void showBlobs(Mat image){
	const char *wndNameOut = "Out";

        Mat out;
        vector<KeyPoint> keyPoints;
        SimpleBlobDetector::Params params;
		params.filterByColor = false;
        params.filterByCircularity = false;

		params.minDistBetweenBlobs = BLOB_DISTANCE_BETWEEEN_BLOBS;
        params.minThreshold = 40;
        params.maxThreshold = 100;
        params.thresholdStep = 10;

        params.minArea = 300; 
        params.maxArea = 8000;
                
		params.minConvexity = 0.3;
        params.maxConvexity = 10;

		params.minInertiaRatio = 0.01;


        SimpleBlobDetector blobDetector( params );
        blobDetector.create("SimpleBlob");

        blobDetector.detect(image, keyPoints);
        drawKeypoints(image, keyPoints, out, CV_RGB(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		for(int i = 0; i < keyPoints.size(); i++) {
			if(i + 1 < keyPoints.size()){
				line(out, keyPoints[i].pt, keyPoints[i + 1].pt, CV_RGB(0, 255, 0), 2);
			}
			else {
				line(out, keyPoints[i].pt, keyPoints[0].pt, CV_RGB(0, 255, 0), 2);
			}
		}


        cout << "Keypoints " << keyPoints.size() << endl;
        
        imshow("Blobs", out);
        waitKey(0);
}

int main()
{
	char filename[MAX_FOLDER_PATH_CHARS] = "";
	
	strcat_s(filename, MAX_FOLDER_PATH_CHARS, INPUT_FOLDER);
	strcat_s(filename, MAX_FOLDER_PATH_CHARS, IMG_CARD);
	Mat im = imread(filename);
    if (im.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;	
    }

	showBlobs(im);
	
    //imshow("Image", im);
    //waitKey(0);
	
	
		
}
