

#ifndef OpenCV_Tutorial_Globals_h
#define OpenCV_Tutorial_Globals_h

// control flags
extern bool computeObject;
extern bool detectObject;
extern bool trackObject;

// Scene
#define PTM_RATIO 32.0
#define SCENE_OFFSET 20

// Colors
#define WHITE_COLOR cv::Scalar(255, 255, 255)
#define BLACK_COLOR cv::Scalar(0)
#define RED_COLOR cv::Scalar(0, 0, 255)
#define GREEN_COLOR cv::Scalar(0, 255, 0)
#define BLUE_COLOR cv::Scalar(255, 0, 0)

#define OUTPUT_STEPS 1
#endif