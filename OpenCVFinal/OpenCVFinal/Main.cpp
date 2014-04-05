#include <fstream>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/features2d/features2d.hpp"

using namespace cv;
using namespace std;

#define BLOB_DISTANCE_BETWEEEN_BLOBS 50.0f
#define BLOB_MIN_AREA 20.0f
#define BLOB_MAX_AREA 500.0f

void blobDetection(const Mat image){
	/*SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = BLOB_DISTANCE_BETWEEEN_BLOBS;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;
	params.filterByArea = true;
	params.minArea = BLOB_MIN_AREA;
	params.maxArea = BLOB_MAX_AREA;*/

	FastFeatureDetector fast;// = new FastFeatureDetector();
	//SimpleBlobDetector *blobDetector = new cv::SimpleBlobDetector(params);
	vector<cv::KeyPoint> keypoints;
	fast.create("FAST");
	fast.detect(image, keypoints);
}

int main()
{
	Mat im = imread("C:\\vs2012_projects\\Inputs\\card.jpg");
    if (im.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;	
    }

	blobDetection(im);
	
    imshow("Image", im);
    waitKey(0);
}
