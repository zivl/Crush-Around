
#include "LcInPaint.h"

using namespace cv;

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

        // now offset them by a few pixels to ensure black edges outside mask
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
        fillPoly(mask, ppt, npt, 1, Scalar(255));

        delete[] pnts;
    }

    // finally, do the inpaint
	//cv::Mat tempInput, tempOutput;
	cv::Mat temp;
	cv::cvtColor(input, temp, CV_BGRA2BGR);
	//cvtColor(input, input, CV_BGRA2BGR);
    cv::inpaint(temp, mask, output, 15, INPAINT_TELEA);
	cvtColor(output, output, CV_BGR2BGRA);

#if defined  _MSC_VER
    cv::imshow("transformed", output);
#endif
}