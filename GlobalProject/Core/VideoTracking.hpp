
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

#include "Core/Globals.h"

// bitwise template operators for int based enums etc
template<class T> inline T operator~ (T a) { return (T)~(int)a; }
template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

class VideoTracking : IBallInSceneObserver, IObjectsDestryedObserver
{
public:
    enum FeatureType { ORB, SIFT, SURF };

    enum GameType { BARRIERS = 1, PADDLES = 2 };

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

    void setGameType(GameType gameType);
    GameType getGameType();
private:

    std::vector<std::function<void()>> m_objectsDestroyedObserversList;
    std::vector<std::function<void()>> m_ballInSceneObserversList;    

    cv::Mat m_nextImg;
    cv::Mat m_mask;

    std::vector<cv::KeyPoint> m_nextKeypoints;
    cv::Mat                   m_nextDescriptors;

    FeatureType m_featureTypeForDetection;

    // ORB based feature detection classes
    cv::ORB                 m_orbFeatureEngine;
    cv::BFMatcher           m_orbMatcher;
    cv::FlannBasedMatcher * m_matcher;

    // SIFT/SURF object detection classes
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
    void calculateHomography(const cv::Mat& inputFrame);

    // transform scene and add to output frame
    void transformScene(cv::Mat& outputFrame);

    // check if ball is in scene considering the ball position (in "physical world") and homography
    // if not, notifies that ball left sence (to observers) and return false if not 
    bool checkBallInScene(int width, int height);

    // create paddles based on the image size
    void createPaddles(const cv::Mat& reference);

    // smooth the homography
    void smoothHomography();

    // the physical world simulation
    World *m_2DWorld;

    // homograph from reference frame to current (last captured) frame
    cv::Mat m_refFrame2CurrentHomography;

    // last homography to smooth homographies and mitigate vibrations between them
    cv::Mat m_lastHomography;

    // inpainted scene used to fill over "destroyed" parts of the image
    cv::Mat m_inpaintedScene;

    // flag indicating whether the ball should be restricted to the scene or can exit
    bool m_restrictBallInScene;

    // flag indicating whether the homography calculation should be based on "good points" only
    bool m_useGoodPointsOnly;

    // type of game - barriers or paddles
    GameType m_GameType;

    // position of the paddles (relative to the screen/frame
    std::vector<cv::Point2f> m_paddlePositions;
};


#endif
