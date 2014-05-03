#include "LcObjectDetector.h"

#include <iostream>

LcObjectDetector::LcObjectDetector(void)
{
}


LcObjectDetector::~LcObjectDetector(void)
{
}

void LcObjectDetector::setObjectSimplification(bool enabled)
{
    this->m_simplifyObjects = enabled;
}

std::vector<std::vector<cv::Point>> LcObjectDetector::getObjectContours(const cv::Mat &image)
{
    std::vector<std::vector<cv::Point>> contours;  
    
    std::vector<std::vector<cv::Point>> objectContours;

    cv::Mat output;
    cv::Canny(image, output, 100, 200);//; showWatershedSegmentation(frame2);

    // try to find countors on the image
    cv::findContours(output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    std::vector<std::vector<ClipperLib::IntPoint>> polygons;

    std::vector<cv::Point> approx;
    // test each contour
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
                        
        if (approx.size() == 4 &&
            fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
            cv::isContourConvex(cv::Mat(approx)) )
        {
            objectContours.push_back(std::vector<cv::Point>(approx));

            // convert to clipper format
            polygons.push_back(convertToClipperFormat(approx));
        }
        else if (approx.size() < 25 &&
                fabs(cv::contourArea(cv::Mat(approx))) > 1000)
        {
            objectContours.push_back(std::vector<cv::Point>(approx));
            // convert to clipper format
            polygons.push_back(convertToClipperFormat(approx));
        }         
        else
        {
            convexHull(contours[i], approx); 
            if (fabs(cv::contourArea(cv::Mat(approx))) > 1000){
                // limit the size
                objectContours.push_back(std::vector<cv::Point>(approx));
                // convert to clipper format
                polygons.push_back(convertToClipperFormat(approx));
            }
        }       
    }

    if (this->m_simplifyObjects)
    {
        polygons = this->simplify(polygons);

        //std::cout << "Detected " << objectContours.size() << " simplified " << polygons.size() << std::endl;
        
        // clear the original list
        objectContours.clear();

        // convert back to the open cv coordinates
        for(size_t i = 0; i < polygons.size(); i++)
        {
            std::vector<cv::Point> poly;
            for (size_t p = 0; p < polygons[i].size(); p++)
            {
                poly.push_back(cv::Point(polygons[i][p].X, polygons[i][p].Y)); 
            }

            objectContours.push_back(poly);
        }
    }

    return objectContours;
}

std::vector<ClipperLib::IntPoint> LcObjectDetector::convertToClipperFormat(const std::vector<cv::Point> &poly)
{
    std::vector<ClipperLib::IntPoint> outpoly(poly.size()); //= new std::vector<ClipperLib::IntPoint>(poly.size());
    
    for(size_t i = 0; i < poly.size(); i++)
    {
        outpoly[i] = ClipperLib::IntPoint(poly[i].x, poly[i].y);
    }

    return outpoly;
}

ClipperLib::Paths LcObjectDetector::simplify(ClipperLib::Paths &polygons)
{
    size_t count = polygons.size();
    // try to simplify the contours by finding intersection and merging them
    bool* processedPolygons = new bool[count]();

    ClipperLib::Paths simplifiedPolygons;

    size_t s = 0;
    while (s < polygons.size())
    {
        ClipperLib::Path currentPoly = polygons[s];
        processedPolygons[s] = true;

        for (size_t t = s + 1; t < polygons.size(); t++)
        {
            if (!processedPolygons[t])
            {
                // test if currentPoly intersects with polygons[i]
                ClipperLib::Clipper clipper;
                clipper.AddPath(currentPoly, ClipperLib::ptSubject, true);
                clipper.AddPath(polygons[t], ClipperLib::ptClip, true);

                // if intersect, merge and update current poly
                ClipperLib::Paths results;
                clipper.Execute(ClipperLib::ctIntersection, results, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                if (results.size() > 0)
                {
                    clipper.Execute(ClipperLib::ctUnion, results, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                    currentPoly = results[0];

                    // mark polygons[i] as processed
                    processedPolygons[t] = true;
                }                
            }
        }

        // save merged polygon
        simplifiedPolygons.push_back(currentPoly);

        // take next un-merge polygon
        s += 1;
        while (processedPolygons[s] && s < polygons.size())
        {
            s+= 1;
        }
    }

    return simplifiedPolygons;
}