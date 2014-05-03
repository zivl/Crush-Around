
#include "LcInPaint.h"

using namespace cv;
/*
void LcInPaint::prepareInpaint(const Mat input){
    input.copyTo(frame2);
    input.copyTo(frame3);

    contours = objDetector.getObjectContours(frame2);

    std::vector<Point> approx;
    // test each contour
    for( size_t i = 0; i < contours.size(); i++ )
    {
        Point* pnts = new Point[contours[i].size()];
        for (int j = 0; j < contours[i].size(); j++)
        {
            pnts[j] = contours[i][j];
        }

        const Point* ppt[1] = { pnts };
        int npt[] = { static_cast<int>(contours[i].size()) };
        fillPoly(frame3, ppt, npt, 1, Scalar(120, 250, 50));

        delete[] pnts;
    }
}*/

void LcInPaint::inpaint(const Mat input, std::vector<std::vector<cv::Point>> contours, Mat &output){
    
    Mat mask;
    // create the mask the same size as the orignal input with 1 channel
    mask.create(input.rows, input.cols, CV_8UC1);
    // set it all to zeros
    mask = Scalar(0);

    std::vector<std::vector<cv::Point>> offsetContours;

    // translate the contours to clipper format 
    
    for( size_t i = 0; i < contours.size(); i++ )
    {
        ClipperLib::Paths polygons;
        ClipperLib::Path polygon;
        for (int j = 0; j < contours[i].size(); j++)
        {
            polygon.push_back(ClipperLib::IntPoint(contours[i][j].x, contours[i][j].y));
        }

        polygons.push_back(polygon);

        // now offset them by a few pixels to ensure no black edges outside mask
        ClipperLib::ClipperOffset clipperOffset;
        clipperOffset.AddPaths(polygons, ClipperLib::jtRound, ClipperLib::etClosedPolygon);

        ClipperLib::Paths offsetPolygons;
        clipperOffset.Execute(offsetPolygons, 3);

        // now, translate back to open cv format
        for( size_t i = 0; i < offsetPolygons.size(); i++ )
        {
            std::vector<cv::Point> offsetContour;

            for (int j = 0; j < offsetPolygons[i].size(); j++)
            {
                offsetContour.push_back(cv::Point((int)offsetPolygons[i][j].X, (int)offsetPolygons[i][j].Y));
            }

            offsetContours.push_back(offsetContour);
        }
    }
  
    // after offsetting all the contours create the mask by painting on the image
    for( size_t i = 0; i < offsetContours.size(); i++ )
    {
        Point* pnts = new Point[offsetContours[i].size()];
        for (int j = 0; j < offsetContours[i].size(); j++)
        {
            pnts[j] = offsetContours[i][j];
        }

        const Point* ppt[1] = { pnts };
        int npt[] = { static_cast<int>(offsetContours[i].size()) };
        fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 255));

        delete[] pnts;
    }

    // finally, do the inpaint
    cv::inpaint(input, mask, output, 15, INPAINT_TELEA);
}