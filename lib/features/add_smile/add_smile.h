#ifndef ADD_SMILE_H
#define ADD_SMILE_H

#include "../BaseFeature.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

using namespace cv;
using namespace std;

typedef vector<int> Triangle;

#define LEFT_EYE_BEGIN 36 // index of leftmost eye on the image
#define LEFT_EYE_END 41
#define RIGHT_EYE_BEGIN 42 // index of rightmost eye on the image
#define RIGHT_EYE_END 47
#define DATA_FILENAME "../shape_predictor_68_face_landmarks.dat"
#define TRIANGLE_FILENAME "../tri.txt"


class AddSmile : public BaseFeature {
public:
    AddSmile(int argc, char** argv);
    void calculateAndDisplay ();
protected:
    Mat imgSrc; // image whose mouth we will extract
    string fileSrc;

    // calculate facial landmarks
    void calculateLandmarks(string filename, vector<Point2f> & output);
};

#endif
