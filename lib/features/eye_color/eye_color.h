#ifndef EYE_COLOR_H
#define EYE_COLOR_H

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


class EyeColor : public BaseFeature {
public:
    EyeColor(int argc, char** argv);
    ~EyeColor();
    void calculateAndDisplay ();
protected:
    Mat imgSrc; // image whose eyes we will extract
    string fileSrc;
    const double alpha = 0.7;

    // calculate facial landmarks
    void calculateLandmarks(string filename, vector<Point2f> & output);

    // Apply affine transform calculated using srcTri and dstTri to src
    void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri);

    // Warps and alpha blends triangular regions from img and srcImg to img
    void morphTriangle(Mat &img1, Mat &img2, Mat &img, vector<Point2f> &t1, vector<Point2f> &t2, vector<Point2f> &t, double alpha);

    // triangularization
    void calculateTriangles(vector<Triangle> & triangles);   
};

#endif