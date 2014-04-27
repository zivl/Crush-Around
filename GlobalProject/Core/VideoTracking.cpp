

#include <iostream>
#include "VideoTracking.hpp"

static const int TrackingAlgorithmKLT           = 0;
static const int TrackingAlgorithmBRIEF         = 1;
static const int TrackingAlgorithmORB           = 2;

VideoTracking::VideoTracking()
: m_orbMatcher(cv::NORM_HAMMING, true)
, m_briefMatcher(cv::NORM_HAMMING, true)
, m_fastDetector(cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector()), 500)
//nfeatures=500, scaleFactor=1.2f, nlevels=8, edgeThreshold=31, firstLevel=0, WTA_K=2, scoreType=ORB::HARRIS_SCORE, patchSize=31
, m_orbFeatureEngine(500, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, 31)
{
    m_maxNumberOfPoints = 50;

    m_detector = cv::FeatureDetector::create("GridFAST");

    std::vector<int> trackingAlgorithms;

    trackingAlgorithms.push_back(TrackingAlgorithmKLT);
    trackingAlgorithms.push_back(TrackingAlgorithmORB);
    trackingAlgorithms.push_back(TrackingAlgorithmBRIEF);

    m_activeTrackingAlgorithm = TrackingAlgorithmORB;
    registerOption("Points count", "ORB", &m_maxNumberOfPoints, 1, 100);

    // TC: instantiate a matcher for descriptor based correlation of features.
    m_matcher = new cv::FlannBasedMatcher(new cv::flann::LshIndexParams(5, 24, 2));

    dt = 1.0f/60.0f;

    this->m_debugDraw = new OpenCvDebugDraw(PTM_RATIO);
}

//! Gets a sample name
std::string VideoTracking::getName() const
{
    return "Video tracking";
}

//! Returns a user-friendly name for displaying as description
std::string VideoTracking::getUserFriendlyName() const
{
    std::stringstream s;
    s << "Video tracking using " << m_activeTrackingAlgorithm;
    return  s.str();
}

//! Returns a detailed sample description
std::string VideoTracking::getDescription() const
{
    return "";
}

std::string VideoTracking::getSampleIcon() const
{
    return "VideoTrackingIcon.png";
}

//! Processes a frame and returns output image
bool VideoTracking::processFrame(const cv::Mat& inputFrame, cv::Mat& outputFrame)
{
    this->m_world->Step(dt, 10, 10);

    inputFrame.copyTo(outputFrame);

    getGray(inputFrame, m_nextImg);

    if (m_activeTrackingAlgorithm == TrackingAlgorithmKLT)
    {
        if (m_mask.rows != inputFrame.rows || m_mask.cols != inputFrame.cols)
        {
            m_mask.create(inputFrame.rows, inputFrame.cols, CV_8UC1);
        }

        if (m_prevPts.size() > 0)
        {
            cv::calcOpticalFlowPyrLK(m_prevImg, m_nextImg, m_prevPts, m_nextPts, m_status, m_error);
        }

        m_mask = cv::Scalar(255);

        std::vector<cv::Point2f> trackedPts;

        for (size_t i=0; i<m_status.size(); i++)
        {
            if (m_status[i])
            {
                trackedPts.push_back(m_nextPts[i]);

                cv::circle(m_mask, m_prevPts[i], 15, cv::Scalar(0), CV_FILLED);
                cv::line(outputFrame, m_prevPts[i], m_nextPts[i], CV_RGB(0,250,0));
                cv::circle(outputFrame, m_nextPts[i], 3, CV_RGB(0,250,0), CV_FILLED);
            }
        }

        bool needDetectAdditionalPoints = trackedPts.size() < m_maxNumberOfPoints;
        if (needDetectAdditionalPoints)
        {
            m_detector->detect(m_nextImg, m_nextKeypoints, m_mask);
            int pointsToDetect = m_maxNumberOfPoints - trackedPts.size();

            if (m_nextKeypoints.size() > pointsToDetect)
            {
                std::random_shuffle(m_nextKeypoints.begin(), m_nextKeypoints.end());
                m_nextKeypoints.resize(pointsToDetect);
            }

            //std::cout << "Detected additional " << m_nextKeypoints.size() << " points" << std::endl;

            for (size_t i=0; i<m_nextKeypoints.size(); i++)
            {
                trackedPts.push_back(m_nextKeypoints[i].pt);
                cv::circle(outputFrame, m_nextKeypoints[i].pt, 5, cv::Scalar(255,0,255), -1);
            }
        }

        m_prevPts = trackedPts;
        m_nextImg.copyTo(m_prevImg);

        // TC: following code blocl was added 2014-04-22
        // transform the sence using optical flow and add to output
        if (m_refPoints.size() > 0)
        {
            // calculate the homography comparing to the reference frame

            // find the optical flow of the points between the reference frame and this one
            cv::calcOpticalFlowPyrLK(m_refFrame, m_nextImg, m_refPoints, m_nextPts, m_status, m_error);

            // loop on the status vector and include add/take points with optical flow
            std::vector<cv::Point2f> refPoints, newPoints;
            for (size_t i=0; i<m_status.size(); i++)
            {
                if (m_status[i])
                {
                    refPoints.push_back(m_refPoints[i]);
                    newPoints.push_back(m_nextPts[i]);
                }
            }

            if(refPoints.size() > 3 && newPoints.size() > 3){

                // finally, find the homography
                cv::Mat H = findHomography( refPoints, newPoints, CV_RANSAC );

                // wrap/transform the scene
                cv::Mat transformedScene;
                warpPerspective(m_scene, transformedScene, H, outputFrame.size(), CV_INTER_LINEAR);

                // add to the output
                outputFrame += transformedScene;
            }
        }

    }
    else if (m_activeTrackingAlgorithm == TrackingAlgorithmORB)
    {
        m_orbFeatureEngine(m_nextImg, cv::Mat(), m_nextKeypoints, m_nextDescriptors);
        calcHomographyAndTransformScene(outputFrame);

    }
    else if(m_activeTrackingAlgorithm == TrackingAlgorithmBRIEF)
    {
        m_fastDetector.detect(m_nextImg, m_nextKeypoints);
        m_briefExtractor.compute(m_nextImg, m_nextKeypoints, m_nextDescriptors);

        // TC: following code is added by Tomer 2014-04-22
        calcHomographyAndTransformScene(outputFrame);
    }

    // add the debug drawing
    this->m_debugDraw->SetScene(outputFrame);
    this->m_world->DrawDebugData();

    //outputFrame += m_scene;

    return true;
}

// TC: following code was added 2014-04-22

//! Sets the reference frame for latter processing
void VideoTracking::setReferenceFrame(const cv::Mat& reference)
{
    // save the reference frame
    reference.copyTo(m_refFrame);

    // get a gray image
    getGray(m_refFrame, m_refFrame);

    // according to the used tracking algorithm,
    // determine how to extract feature/keypoint/descriptors/points to use for tracking
    if (m_activeTrackingAlgorithm == TrackingAlgorithmKLT)
    {
        // create the mask if not yet created
        if (m_mask.rows != reference.rows || m_mask.cols != reference.cols)
        {
            m_mask.create(reference.rows, reference.cols, CV_8UC1);
        }

        // set it's color
        m_mask = cv::Scalar(255);

        // detect key points in the reference frame
        m_detector->detect(m_refFrame, m_refKeypoints, m_mask);

        // push the points into reference points vector
        for (size_t i = 0; i < m_refKeypoints.size(); i++)
        {
            m_refPoints.push_back(m_refKeypoints[i].pt);
        }
    }
    else if (m_activeTrackingAlgorithm == TrackingAlgorithmORB)
    {
        // detect key points and generate descriptors for the reference frame
        m_orbFeatureEngine(m_refFrame, cv::Mat(), m_refKeypoints, m_refDescriptors);
    }
    else if(m_activeTrackingAlgorithm == TrackingAlgorithmBRIEF)
    {
        // detect key points and extract descriptors for the reference frame
        m_fastDetector.detect(m_refFrame, m_refKeypoints);
        m_briefExtractor.compute(m_refFrame, m_refKeypoints, m_refDescriptors);
    }

    // create the scene and draw square and borders in it
    m_scene.create(reference.rows, reference.cols, CV_8UC3);
    m_scene = cv::Scalar(0,0,0);

    // draw a square in the center
    rectangle(m_scene, cvPoint(reference.cols/2 - 20, reference.rows/2 + 10), cvPoint(reference.cols/2 + 20, reference.rows /2 - 10), cv::Scalar(0,255,255), -1);

    // draw a border 20 pixels into the image
    line(m_scene, cvPoint(20, 20), cvPoint(20, reference.rows - 20), cv::Scalar(0, 255, 0), 2);
    line(m_scene, cvPoint(20, reference.rows - 20), cvPoint(reference.cols - 20, reference.rows - 20), cv::Scalar(0, 255, 0), 2);
    line(m_scene, cvPoint(reference.cols - 20, reference.rows - 20), cvPoint(reference.cols - 20, 20), cv::Scalar(0, 255, 0), 2);
    line(m_scene, cvPoint(reference.cols - 20, 20), cvPoint(20, 20), cv::Scalar(0, 255, 0), 2);

    // TC: following is box2d world/ball initialization
    // define world with gravity
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    m_world = new b2World(gravity);

    m_world->SetDebugDraw(this->m_debugDraw);

    this->m_debugDraw->SetFlags( b2Draw::e_shapeBit );

    // Create edges around the entire screen
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0,0);

    cv::Size visibleSize(reference.cols, reference.rows);

    b2Body *groundBody = m_world->CreateBody(&groundBodyDef);
    b2EdgeShape groundEdge;
    b2FixtureDef boxShapeDef;
    boxShapeDef.shape = &groundEdge;

    // wall definitions - bottom
    groundEdge.Set(b2Vec2(0,0), b2Vec2(visibleSize.width/PTM_RATIO, 0));
    groundBody->CreateFixture(&boxShapeDef);

    // left
    groundEdge.Set(b2Vec2(0,0), b2Vec2(0, visibleSize.height/PTM_RATIO));
    groundBody->CreateFixture(&boxShapeDef);

    // top
    groundEdge.Set(b2Vec2(0, visibleSize.height/PTM_RATIO),
                   b2Vec2(visibleSize.width/PTM_RATIO, visibleSize.height/PTM_RATIO));
    groundBody->CreateFixture(&boxShapeDef);

    // right
    groundEdge.Set(b2Vec2(visibleSize.width/PTM_RATIO, visibleSize.height/PTM_RATIO),
                   b2Vec2(visibleSize.width/PTM_RATIO, 0));
    groundBody->CreateFixture(&boxShapeDef);

    // Create ball body and shape
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(100/PTM_RATIO, 100/PTM_RATIO);
    m_ballBody = m_world->CreateBody(&ballBodyDef);

    b2CircleShape circle;
    circle.m_radius = 26.0/PTM_RATIO;

    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 2.5f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = 1.0f;
    m_ballFixture = m_ballBody->CreateFixture(&ballShapeDef);

    m_ballBody->ApplyLinearImpulse(b2Vec2(100, 100), m_ballBody->GetPosition(), true);

}

// calculate homography for keypoint/descriptors based tracking,
// find matching (corresponding) features (using open CV built in matcher)
// calculate the homograpgy (using open CV built in function that uses RANSAC)
// then transform the scene and add it to the output frame
void VideoTracking::calcHomographyAndTransformScene(cv::Mat& outputFrame)
{
    // transform the sence using descriptors, correspondance and homography
    if (m_refKeypoints.size() > 0)
    {
        // create vector for matches and find matches between reference and new descriptors
        std::vector<cv::DMatch> matches;

        // m_refDescriptors is keypoint descriptors from the first frame
        // m_nextDescriptors is keypoint descriptors from the current processed frame
        m_matcher->match(m_refDescriptors, m_nextDescriptors, matches);

        double max_dist = 0; double min_dist = 100;

        // Quick calculation of max and min distances between keypoints
        for( int i = 0; i < matches.size(); i++ )
        {
            double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        //        std::cout << "-- Max dist : " << max_dist << std::endl;
        //        std::cout << "-- Min dist : " << min_dist << std::endl;

        // Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
        std::vector< cv::DMatch > good_matches;

        // min dist could be 0 so make it slightly larger if zero
        min_dist = std::max(min_dist, 0.5);

        for( int i = 0; i < matches.size(); i++ )
        {
            if( matches[i].distance < 3 * min_dist ) {
                good_matches.push_back( matches[i]);
            }
        }

        // Localize the object
        std::vector<cv::Point2f> refPoints, newPoints;

        for( int i = 0; i < good_matches.size(); i++ )
        {
            // Get the keypoints from the good matche (reference and new)
            refPoints.push_back(m_refKeypoints[good_matches[i].queryIdx].pt);
            newPoints.push_back( m_nextKeypoints[good_matches[i].trainIdx].pt);
        }

        if (refPoints.size() > 3 && newPoints.size() > 3)
        {
            // finally, find the homography
            this->m_refFrame2CurrentHomography = findHomography(refPoints, newPoints, CV_RANSAC);

            // wrap/transform the scene
            cv::Mat transformedScene;
            m_scene.copyTo(transformedScene);

            for (int i = 0; i < this->m_destroyedPoints.size(); i++)
            {
                cv::circle(transformedScene, this->m_destroyedPoints[i], 5, cv::Scalar(200, 200, 200), -1);
            }
            
            cv::circle(transformedScene,
                       cv::Point2f(m_ballBody->GetPosition().x * PTM_RATIO, m_ballBody->GetPosition().y * PTM_RATIO),
                       26, cv::Scalar(255, 0, 0), -1);

            warpPerspective(transformedScene, transformedScene, this->m_refFrame2CurrentHomography, outputFrame.size(), CV_INTER_LINEAR);

            // add to the output
            outputFrame += transformedScene;
        }
    }
}

void VideoTracking::onMouse( int event, int x, int y, int, void* )
{
    if (event != CV_EVENT_LBUTTONDOWN || this->m_refFrame2CurrentHomography.empty()){
        return;
    }

    std::vector<cv::Point2f> sourcePoints;
    sourcePoints.push_back(cv::Point2f(x, y));

    std::vector<cv::Point2f> targetPoints;

    cv::perspectiveTransform(sourcePoints, targetPoints, this->m_refFrame2CurrentHomography.inv());

    // add to the world model
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(targetPoints[0].x/PTM_RATIO, targetPoints[0].y/PTM_RATIO);
    std::cout << "[" << targetPoints[0].x/PTM_RATIO << "," <<targetPoints[0].y/PTM_RATIO << "]" << std::endl;
    b2Body *body = m_world->CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = 5.0/PTM_RATIO;

    b2FixtureDef shapeDef;
    shapeDef.shape = &circle;
    shapeDef.density = 10.0f;
    shapeDef.friction = 0;
    body->CreateFixture(&shapeDef);

    // add the target point to the list of blacked areas
    m_destroyedPoints.push_back(targetPoints[0]);
}

void VideoTracking::mouseCallback(int event, int x, int y, int flags, void *param)
{
    VideoTracking *self = static_cast<VideoTracking*>(param);
    self->onMouse(event, x, y, flags, param);
}

void VideoTracking::setObjectsToBeModeled(const std::vector<std::vector<cv::Point>> contours) {

    int contourSize = (int)contours.size();
    for(int i = 0; i < contourSize/*1*/; i++ )
    {
        std::vector<cv::Point> currentShape = contours[i];
        int numOfPoints = (int)currentShape.size();
        if(numOfPoints <= 8)
        {
//			std::cout << "Shape: " << i << " Points: " << numOfPoints << std::endl;
            b2Vec2 vertices[8];
            for (int j = 0; j < numOfPoints; j++)
            {
                vertices[j].x = currentShape[j].x / PTM_RATIO;
                vertices[j].y = currentShape[j].y / PTM_RATIO;

                cv::line(m_scene, currentShape[j], currentShape[(j + 1) % numOfPoints], cv::Scalar(0,0,255));
                std::cout << "[" << vertices[j].x << "," <<vertices[j].y << "]" << std::endl;
            }

            b2PolygonShape polygon;
            polygon.Set(vertices, numOfPoints);

            b2BodyDef objectBodyDef;
            objectBodyDef.type = b2_staticBody;
            //objectBodyDef.position.Set(polygon.m_centroid.x, polygon.m_centroid.y);
            std::cout << "center=[" << polygon.m_centroid.x << "," << polygon.m_centroid.y << "]" << std::endl;
            b2Body *objectBody = m_world->CreateBody(&objectBodyDef);

            b2FixtureDef objectShapeDef;
            objectShapeDef.shape = &polygon;
            objectShapeDef.density = 10.0f;
            objectShapeDef.friction = 0.0f;
            objectBody->CreateFixture(&objectShapeDef);
        }
    }
}










