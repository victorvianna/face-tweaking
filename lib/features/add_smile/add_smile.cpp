#include "add_smile.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
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

/*
 * This function returns the mid point of a set of points
 *
 * @param points a vector o 2-dimensional points
 * @return a 2-bidimensional point that is the medium
 * point of the points passed as argument
*/
cv::Point get_mid_point(std::vector<cv::Point> &points) {
	int xc = 0,
            yc = 0;
	
	for (cv::Point p : points) {
		xc += p.x;
		yc += p.y;
	}
	
	xc /= points.size();
	yc /= points.size();

	return cv::Point(xc, yc);
}

AddSmile::AddSmile (int argc, char** argv) : BaseFeature(argc, argv)
{
    if(argc<4)
        throw("Number of arguments is not enough");
    fileSrc = string(argv[3]);
    imgSrc = imread(fileSrc);
}       

void AddSmile::calculateAndDisplay ()
{        
    //imgIn.convertTo(imgIn, CV_32F); // contains face image
    //imgSrc.convertTo(imgSrc, CV_32F); // contains smile image

    //Calculate facial landmarks
    vector<Point2f> pointsIn;
    calculateLandmarks(fileIn, pointsIn);
    
    vector<Point> mouth_points;
    for (int i = 48; i < pointsIn.size(); i++) { // mouth's points are have indices in the interval [48..67]
        mouth_points.push_back(Point((int) pointsIn[i].x, (int) pointsIn[i].y));	
    }

    Point center = get_mid_point(mouth_points);
	Rect bounding_rect = boundingRect(mouth_points);

	double fx = 0.5, // factor of scaling for the width of the smile image
		   fy = 0.5; // factor of scaling for the height of the smile image
	//cin >> fx >> fy;
    resize(imgSrc, imgSrc, Size2i(0, 0), fx, fy);
    
    // Create a rough mask around the image to be inserted (in our case a mouth).
    Mat src_mask = Mat(imgSrc.rows, imgSrc.cols, imgSrc.depth(), Scalar(255,255,255));
    
    seamlessClone(imgSrc, imgIn, src_mask, center, imgOut, NORMAL_CLONE);
    
    // Display Result
    imshow("Image with smile added", imgOut);
    waitKey(0);
}



// calculate facial landmarks
void AddSmile::calculateLandmarks(string filename, vector<Point2f> & output)
{
    ifstream ifs(filename+".txt");
    float x, y;
    while(ifs >> x >> y)
    {
        output.push_back(Point2f(x,y));
    }

    if(output.size()!=0) // if points were already calculated, just load them
    return;

    // otherwise:
    // USE DLIB'S FACE_LANDMARK DETECTION

    ofstream ofs(filename+".txt");

    // We need a face detector.  We will use this to get bounding boxes for
    // each face in an image.
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    // And we also need a shape_predictor.  This is the tool that will predict face
    // landmark positions given an image and face bounding box.  Here we are just
    // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
    // as a command line argument.
    dlib::shape_predictor sp;


    dlib::deserialize(DATA_FILENAME) >> sp;

    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, filename);
    // Make the image larger so we can detect small faces.

    //dlib::pyramid_up(img); // if we enlarge the image we have problems

    // Now tell the face detector to give us a list of bounding boxes
    // around all the faces in the image.
    vector<dlib::rectangle> dets = detector(img);
    //cout << "Number of faces detected: " << dets.size() << endl;

    assert(dets.size()==1);  /// WE HAVE TO TREAT LATER THE CASE WHERE THERE ARE MULTIPLE FACES

    // Now we will go ask the shape_predictor to tell us the pose of
    // each face we detected.
    //std::vector<full_object_detection> shapes;
    for (unsigned long j = 0; j < dets.size(); ++j) {
        dlib::full_object_detection shape = sp(img, dets[j]);

        for (int i = 0; i < shape.num_parts(); i++) {
            output.push_back(Point2f((float) shape.part(i).x(), (float) shape.part(i).y()));
        }

    }

    // write file

    for(Point2f & p : output)
    {
       ofs << p.x<<" "<<p.y<<endl;
    }
}

