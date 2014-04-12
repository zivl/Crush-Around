#include "OpenCVUtils.h"

void OpenCVUtils::getBinMask( const Mat& comMask, Mat& binMask )
{
    if( comMask.empty() || comMask.type()!=CV_8UC1 ){
		// should we manage a log file?
		//cout << "comMask is empty or has incorrect type (not CV_8UC1)" << endl;
	}
    if( binMask.empty() || binMask.rows!=comMask.rows || binMask.cols!=comMask.cols ){
        binMask.create( comMask.size(), CV_8UC1 );
	}
    binMask = comMask & 1;
}