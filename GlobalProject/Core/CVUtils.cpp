//
//  CVUtils.cpp
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/13/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#include "CVUtils.h"


cv::Point2f CVUtils::transformPoint(const cv::Point2f point, const cv::Mat homoMat) {

	std::vector<cv::Point2f> sourcePoints;
    sourcePoints.push_back(point);

    std::vector<cv::Point2f> targetPoints;

    cv::perspectiveTransform(sourcePoints, targetPoints, homoMat);
    return cv::Point2f(targetPoints[0]);	
}

void CVUtils::transformPoints(const std::vector<cv::Point2f> points, std::vector<cv::Point2f>* targetPoints, const cv::Mat homography) {
    cv::perspectiveTransform(points, (*targetPoints), homography);
}