
#ifndef OpenCV_Tutorial_VideoTracking_hpp
#define OpenCV_Tutorial_VideoTracking_hpp

#include "SampleBase.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/calib3d/calib3d.hpp>         //for homography
#include <opencv2/features2d/features2d.hpp>   //features (orb, brief and corresponding matcher/extractors)
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <functional>
#include <Box2D/Box2D.h>

#include "clipper.hpp"

#include "OpenCvDebugDraw.h"
#include "MyContactListener.h"
#include "LcInPaint.h"
//#include "IBallHitObserver.h"


#define PTM_RATIO 32.0



enum {
	BALL_HIT_EVENT = 0
};

class VideoTracking //: public SampleBase//, IBallHitObserver
{
public:
    VideoTracking();
    ~VideoTracking();

    virtual void getGray(const cv::Mat& input, cv::Mat& gray);

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

	virtual void onPanGestureEnded(std::vector<cv::Point> touchPoints);

    static void mouseCallback(int event, int x, int y, int flags, void *param);

    void setObjectsToBeModeled(const std::vector<std::vector<cv::Point> > contours);
    
    void prepareInPaintedScene(const cv::Mat scene, const std::vector<std::vector<cv::Point> > contours);

    void setDebugDraw(bool enabled);

	// observer pattern methods for events

	virtual void attachBallHitObserver(std::function<void(float x, float y)> func);
    virtual void detachBallHitObserver(std::function<void(float x, float y)> func);
    virtual void notifyBallHitObservers(float x, float y);
private:

	std::vector<std::function<void(float x, float y)>> observersList;
	
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
    b2Body * m_groundBody;
    b2Fixture * m_groundFixture;

    float dt;

    cv::Mat m_refFrame2CurrentHomography;

    std::vector<cv::Point2f*> m_destroyedPoints;

    OpenCvDebugDraw* m_debugDraw;

     // Contact listener for colision response
    MyContactListener *m_contactListener;   

    // inpainted scene used to fill over "destroyed" parts of the image
    cv::Mat m_inpaintedScene;

    // a list of destroyed regions
    std::vector<cv::Point*> m_destroyedPolygons;
    std::vector<int> m_destroyedPolygonsPointCount;

    bool m_debugDrawEnabled;

	std::vector<b2Body *>m_objectBodies;
};


#endif
