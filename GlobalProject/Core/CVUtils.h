//
//  CVUtils.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#ifndef __DestroyAroundMe__CVUtils__
#define __DestroyAroundMe__CVUtils__

#include <opencv2/core/core.hpp>

class CVUtils {

public:
    static cv::Point2f transformPoint(const cv::Point2f point, const cv::Mat homoMat);
    static void transformPoints(const std::vector<cv::Point2f> points, std::vector<cv::Point2f>* targetPoints, const cv::Mat homography);
};

#endif /* defined(__DestroyAroundMe__CVUtils__) */
