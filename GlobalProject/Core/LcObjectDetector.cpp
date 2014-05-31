#include "LcObjectDetector.h"

#include <iostream>

LcObjectDetector::LcObjectDetector(void)
{
}


LcObjectDetector::~LcObjectDetector(void)
{
}

// apply canny edge detector and threshold to find edges and tranform them to contours
// then simplify the contours, merging overlapping/intersecting polygons to one
std::vector<std::vector<cv::Point> > LcObjectDetector::getObjectContours(const cv::Mat &image)
{
    std::vector<std::vector<cv::Point> > contours;
    
    std::vector<std::vector<cv::Point> > objectContours;

    cv::Mat output;
    cv::Mat gray;
    cv::cvtColor(image, gray, image.channels() == 3 ? CV_BGR2GRAY : CV_BGRA2GRAY);

    // perform canny edge detectors with the specified thresholds
    // TODO: move threshold to be class fields with get/set.
    cv::Canny(gray, output, 100, 200);

#if defined _MSC_VER && OUTPUT_STEPS
    cv::imshow("canny", output);
#endif
    /*
    cv::Mat kernel = cv::getStructuringElement(CV_SHAPE_RECT, cv::Size(5, 5));

    // do openning
    cv::morphologyEx(output, output, CV_MOP_OPEN, kernel);//, cv::Point(-1,-1), 5);

#if defined _MSC_VER
    cv::imshow("opened", output);
#endif

    // do closing
    cv::morphologyEx(output, output, CV_MOP_CLOSE, kernel);//, cv::Point(-1,-1), 2);

#if defined _MSC_VER
    cv::imshow("closed", output);
#endif
    */

    // try to find countors on the image
    cv::findContours(output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    std::vector<std::vector<ClipperLib::IntPoint> > polygons;
    
    // Test each contour
    std::vector<cv::Point> approx;
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
                        
        // this means squares (todo: remove?)
        if (approx.size() == 4 &&
            fabs(cv::contourArea(cv::Mat(approx))) > 500 &&
            cv::isContourConvex(cv::Mat(approx)) )
        {
            objectContours.push_back(std::vector<cv::Point>(approx));

            // convert to clipper format
            polygons.push_back(convertToClipperFormat(approx));
        }
        else if (approx.size() < 40 &&
                fabs(cv::contourArea(cv::Mat(approx))) > 500)
        {
            // non complex polygons
            objectContours.push_back(std::vector<cv::Point>(approx));

            // convert to clipper format
            polygons.push_back(convertToClipperFormat(approx));
        }         
        else
        {
            // simplify the shape to it's convex hull
            convexHull(contours[i], approx); 
            if (fabs(cv::contourArea(cv::Mat(approx))) > 500){
                // limit the size
                objectContours.push_back(std::vector<cv::Point>(approx));
                // convert to clipper format
                polygons.push_back(convertToClipperFormat(approx));
            }
        }       
    }

    polygons = this->simplify(polygons);

    // now offset them by a few pixels to ensure black edges outside mask
    ClipperLib::ClipperOffset clipperOffset;
    clipperOffset.AddPaths(polygons, ClipperLib::jtRound, ClipperLib::etClosedPolygon);

    ClipperLib::Paths offsetPolygons;
    clipperOffset.Execute(offsetPolygons, 1);

    //std::cout << "Detected " << objectContours.size() << " simplified " << polygons.size() << std::endl;

    // clear the original list
    objectContours.clear();

    // convert back to the open cv coordinates
    for(size_t i = 0; i < offsetPolygons.size(); i++)
    {
        std::vector<cv::Point> poly;
        for (size_t p = 0; p < offsetPolygons[i].size(); p++)
        {
            poly.push_back(cv::Point((int)offsetPolygons[i][p].X, (int)offsetPolygons[i][p].Y));
        }

        objectContours.push_back(poly);
    }


    return objectContours;
}

// simplify the polygons in input merging overlapping/intersecting polygons to one
ClipperLib::Paths LcObjectDetector::simplify(ClipperLib::Paths &polygons)
{
    size_t count = polygons.size();
    // try to simplify the contours by finding intersection and merging them
    bool* processedPolygons = new bool[count]();

    // list to save the results (merged/simplified polygons)
    ClipperLib::Paths simplifiedPolygons;
    
    size_t s = 0;

    // loop while not all (original) polygons were tested
    while (s < polygons.size())
    {
        // get the current, non-merged polygon
        ClipperLib::Path currentPoly = polygons[s];
        // and mark it as processed
        processedPolygons[s] = true;

        // loop on all the other original polygons
        for (size_t t = s + 1; t < polygons.size(); t++)
        {
            // if the polygon was not processed (merged)
            if (!processedPolygons[t])
            {
                // test intersection between current and t-th polygon:

                // define a clipper and add the paths
                ClipperLib::Clipper clipper;
                clipper.AddPath(currentPoly, ClipperLib::ptSubject, true);
                clipper.AddPath(polygons[t], ClipperLib::ptClip, true);

                // if intersect, merge and update current poly
                ClipperLib::Paths results;
                clipper.Execute(ClipperLib::ctIntersection, results, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                // if results count > 0 means the intersections is non-empty
                if (results.size() > 0)
                {
                    // so find the union between the two polygons
                    clipper.Execute(ClipperLib::ctUnion, results, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

                    // the union must be the first item in the results (actually should be only one item in the results
                    currentPoly = results[0];

                    // mark the target polygons (polygons[i]) as processed so it won't be considered again
                    processedPolygons[t] = true;
                }                
            }
        }

        // save the merged polygon
        simplifiedPolygons.push_back(currentPoly);

        // take next un-merge (non-processed) polygon
        s += 1;
        while (processedPolygons[s] && s < polygons.size())
        {
            s+= 1;
        }
    }

    return simplifiedPolygons;
}

// convert a vector of cvPoints to vector of clipper points
// TODO: move to unit converter
std::vector<ClipperLib::IntPoint> LcObjectDetector::convertToClipperFormat(const std::vector<cv::Point> &poly)
{
    std::vector<ClipperLib::IntPoint> outpoly(poly.size()); //= new std::vector<ClipperLib::IntPoint>(poly.size());
    
    for(size_t i = 0; i < poly.size(); i++)
    {
        outpoly[i] = ClipperLib::IntPoint(poly[i].x, poly[i].y);
    }

    return outpoly;
}

