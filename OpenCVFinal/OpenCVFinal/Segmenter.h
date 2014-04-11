
#include <opencv2/opencv.hpp>
using namespace cv;

class Segmenter
{

public:
	virtual ~Segmenter(void) {}

	virtual Mat findSegments(Mat &image);
};

