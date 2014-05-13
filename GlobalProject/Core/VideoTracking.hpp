
#ifndef OpenCV_Tutorial_VideoTracking_hpp
#define OpenCV_Tutorial_VideoTracking_hpp

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/calib3d/calib3d.hpp>         //for homography
#include <opencv2/features2d/features2d.hpp>   //features (orb, brief and corresponding matcher/extractors)
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/nonfree/nonfree.hpp"         // SURF detector 
#include <vector>
#include <functional>
#include "World.h"
#include "Globals.h"
#include "LcInPaint.h"
#include "Observers/IBallInSceneObserver.h"
#include "Observers/IObjectsDestryedObserver.h"
#include "CVUtils.h"

class VideoTracking : IBallInSceneObserver, IObjectsDestryedObserver
{
public:
    enum FeatureType { ORB, SIFT, SURF };

    VideoTracking();
    ~VideoTracking();

    // get the martix as grayscale 
    void getGray(const cv::Mat& input, cv::Mat& gray);

    // set the feature type to use for detection
    void setFeatureType(FeatureType feat_type);

    World* getWorld();

    //! Processes a frame and returns output image
    virtual bool processFrame(const cv::Mat& inputFrame, cv::Mat& outputFrame);

    //! Sets the reference frame for latter processing
    virtual void setReferenceFrame(const cv::Mat& reference);

    virtual void onMouse(int event, int x, int y, int, void*);

    virtual void onPanGestureEnded(std::vector<cv::Point> touchPoints);

    static void mouseCallback(int event, int x, int y, int flags, void *param);
    
    void prepareInPaintedScene(const cv::Mat scene, const std::vector<std::vector<cv::Point> > contours);

    void setRestrictBallInScene(bool restricted);
    bool isRestrictBallInScene();

    virtual void delegateBallHitObserver(std::function<void(float x, float y)> func);

    // IObjectsDestroyedObserver observer pattern methods for events
    virtual void attachObjectsDestryedObserver(std::function<void()> func);
    virtual void detachObjectsDestryedObserver(std::function<void()> func);
    virtual void notifyObjectsDestryedObservers();

    // IBallInScenceObserver observer pattern methods for events
    virtual void attachBallInSceneObserver(std::function<void()> func);
    virtual void detachBallInSceneObserver(std::function<void()> func);
    virtual void notifyBallInSceneObservers();

private:

    std::vector<std::function<void()>> objectsDestroyedObserversList;

    std::vector<std::function<void()>> ballInSceneObserversList;    

    cv::Mat m_nextImg;
    cv::Mat m_mask;

    std::vector<cv::KeyPoint> m_nextKeypoints;
    cv::Mat                   m_nextDescriptors;

    FeatureType m_featureTypeForDetection;

    cv::ORB                 m_orbFeatureEngine;
    cv::BFMatcher           m_orbMatcher;
    cv::FlannBasedMatcher * m_matcher;

    cv::SurfFeatureDetector m_surfDetector;
    cv::SurfDescriptorExtractor m_surfExtractor;
    cv::FlannBasedMatcher m_surfMatcher;

    cv::SIFT m_siftEngine;


    // the original (first) frame.
    cv::Mat m_refFrame;
    // key points in the original frame (for feature/descriptors based tracking)
    std::vector<cv::KeyPoint> m_refKeypoints;
    // descriptors of keypoints in the original frame (for feature/descriptors based tracking)
    cv::Mat m_refDescriptors;
   
    // the actual scene corresponding to original frame
    cv::Mat m_scene;

    // Calculate homography for reference and new features,
    // transform scene and add to output frame
    void calcHomographyAndTransformScene(cv::Mat& outputFrame);

    World *m_2DWorld;
    

    // homograph from reference frame to current (last captured) frame
    cv::Mat m_refFrame2CurrentHomography;

    // inpainted scene used to fill over "destroyed" parts of the image
    cv::Mat m_inpaintedScene;

    // flag indicating whether the ball should be restricted to the scene or can exit
    bool m_restrictBallInScene;

    bool m_useGoodPointsOnly;
};


#endif
