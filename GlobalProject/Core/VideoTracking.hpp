
#ifndef OpenCV_Tutorial_VideoTracking_hpp
#define OpenCV_Tutorial_VideoTracking_hpp

#include "SampleBase.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/calib3d/calib3d.hpp> //for homography
#include <opencv2/features2d/features2d.hpp>    //features (orb, brief and corresponding matcher/extractors)
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <Box2D/Box2D.h>

#include "OpenCvDebugDraw.h"

#define PTM_RATIO 32.0

class VideoTracking : public SampleBase
{
public:
    VideoTracking();

    //! Gets a sample name
    virtual std::string getName() const;

    //! Returns a user-friendly name for displaying as description
    virtual std::string getUserFriendlyName() const;

    //! Returns a detailed sample description
    virtual std::string getDescription() const;

    //! Processes a frame and returns output image
    virtual bool processFrame(const cv::Mat& inputFrame, cv::Mat& outputFrame);

    //! Sets the reference frame for latter processing
    virtual void setReferenceFrame(const cv::Mat& reference);

    virtual std::string getSampleIcon() const;

    virtual void onMouse(int event, int x, int y, int, void*);

    static void mouseCallback(int event, int x, int y, int flags, void *param);

    void setObjectsToBeModeled(const std::vector<std::vector<cv::Point>> contours);
    
private:
    int m_maxNumberOfPoints;

    cv::Mat m_prevImg;
    cv::Mat m_nextImg;
    cv::Mat m_mask;

    std::vector<cv::Point2f>  m_prevPts;
    std::vector<cv::Point2f>  m_nextPts;

    std::vector<cv::KeyPoint> m_prevKeypoints;
    std::vector<cv::KeyPoint> m_nextKeypoints;

    cv::Mat                   m_prevDescriptors;
    cv::Mat                   m_nextDescriptors;

    std::vector<unsigned char> m_status;
    std::vector<float>         m_error;

    cv::ORB       m_orbFeatureEngine;
    cv::BFMatcher m_orbMatcher;

    cv::GridAdaptedFeatureDetector m_fastDetector;
    cv::BriefDescriptorExtractor m_briefExtractor;
    cv::BFMatcher                m_briefMatcher;

    //TC: following is all addition by Tomer - 2014-04-22
    cv::Ptr<cv::FeatureDetector> m_detector;
    int m_activeTrackingAlgorithm;

    // the original (first) frame.
    cv::Mat m_refFrame;
    // key points in the original frame (for feature/descriptors based tracking)
    std::vector<cv::KeyPoint> m_refKeypoints;
    // descriptors of keypoints in the original frame (for feature/descriptors based tracking)
    cv::Mat m_refDescriptors;

    cv::FlannBasedMatcher * m_matcher;
    cv::OrbDescriptorExtractor m_orbExtractor;

    // reference points for optical flow/KLT tracking
    std::vector<cv::Point2f>  m_refPoints;

    // the actual scene corresponding to original frame
    cv::Mat m_scene;

    // Calculate homography for reference and new features,
    // transform scene and add to output frame
    void calcHomographyAndTransformScene(cv::Mat& outputFrame);

    // box2d "world" objects
    b2World * m_world;
    b2Body * m_ballBody;
    b2Fixture * m_ballFixture;
    float dt;

    cv::Mat m_refFrame2CurrentHomography;

    cv::vector<cv::Point2f> m_destroyedPoints;

    OpenCvDebugDraw* m_debugDraw;

};


#endif