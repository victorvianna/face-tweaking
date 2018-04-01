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

class AddSmile : public BaseFeature {
public:
    AddSmile(int argc, char** argv);
    void calculateAndDisplay ();
protected:
    Mat imgSrc; // image whose mouth we will extract
    string fileSrc;
    const string DATA_FILENAME = string("lib/shape_predictor_68_face_landmarks.dat");
    const string TRIANGLE_FILENAME = string("lib/features/AddSmile/tri.txt");

    // calculate facial landmarks
    void calculateLandmarks(string filename, vector<Point2f> & output);
};

#endif
