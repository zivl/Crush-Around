#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main()
{
    Mat im = imread("c:\\times.jpg");
    if (im.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;
    }
    imshow("Image", im);
    waitKey(0);
}