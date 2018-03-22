#ifndef BASE_FEATURE_H
#define BASE_FEATURE_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string> 

using namespace std;
using namespace cv;

class BaseFeature
{
protected:
	Mat imgIn, imgOut;
	string fileIn;
public:
	BaseFeature(int argc, char** argv);
	~BaseFeature();
	virtual void calculateAndDisplay();
};

#endif
