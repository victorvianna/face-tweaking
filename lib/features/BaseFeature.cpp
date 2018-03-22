#include "BaseFeature.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string> 

using namespace std;
using namespace cv;

BaseFeature::BaseFeature(int argc, char** argv)
{
	if(argc<3)
		throw("Number of arguments is not enough");
	fileIn = string(argv[2]);
	imgIn = imread(fileIn);
}

BaseFeature::~BaseFeature(){

}




