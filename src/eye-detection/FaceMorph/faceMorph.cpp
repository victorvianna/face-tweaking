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
//#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>

using namespace cv;
//using namespace dlib;
using namespace std;

typedef vector<int> Triangle;

#define LEFT_EYE_BEGIN 36 // leftmost eye on the image
#define LEFT_EYE_END 41
#define RIGHT_EYE_BEGIN 42 //rightmost eye on the image
#define RIGHT_EYE_END 47
#define DATA_FILENAME "../shape_predictor_68_face_landmarks.dat"
#define TRIANGLE_FILENAME "../tri.txt"

// calculate facial landmarks
void calculateLandmarks(string filename, vector<Point2f> & output)
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
    dlib::pyramid_up(img);

    // Now tell the face detector to give us a list of bounding boxes
    // around all the faces in the image.
    vector<dlib::rectangle> dets = detector(img);
    cout << "Number of faces detected: " << dets.size() << endl;

    assert(dets.size()==1);  /// WE HAVE TO TREAT LATER THE CASE WHERE THERE ARE MULTIPLE FACES

    // Now we will go ask the shape_predictor to tell us the pose of
    // each face we detected.
    //std::vector<full_object_detection> shapes;
    for (unsigned long j = 0; j < dets.size(); ++j) {
        dlib::full_object_detection shape = sp(img, dets[j]);
        cout << "number of parts: " << shape.num_parts() << endl;
        cout << "pixel position of first part:  " << shape.part(0) << endl;
        cout << "pixel position of second part: " << shape.part(1) << endl;

        for (int i = LEFT_EYE_BEGIN; i <= LEFT_EYE_END; i++) {
            output.push_back(Point2f((float) shape.part(i).x(), (float) shape.part(i).y()));
        }

        for (int i = LEFT_EYE_BEGIN; i <= LEFT_EYE_END; i++) {
            output.push_back(Point2f((float) shape.part(i).x(), (float) shape.part(i).y()));
        }

    }

    // write file

    for(Point2f & p : output)
    {
       ofs << p.x<<" "<<p.y<<endl;
    }
}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri)
{
    
    // Given a pair of triangles, find the affine transform.
    Mat warpMat = getAffineTransform( srcTri, dstTri );
    
    // Apply the Affine Transform just found to the src image
    warpAffine( src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void morphTriangle(Mat &img1, Mat &img2, Mat &img, vector<Point2f> &t1, vector<Point2f> &t2, vector<Point2f> &t, double alpha)
{
    
    // Find bounding rectangle for each triangle
    Rect r = boundingRect(t);
    Rect r1 = boundingRect(t1);
    Rect r2 = boundingRect(t2);
    
    // Offset points by left top corner of the respective rectangles
    vector<Point2f> t1Rect, t2Rect, tRect;
    vector<Point> tRectInt;
    for(int i = 0; i < 3; i++)
    {
        tRect.push_back( Point2f( t[i].x - r.x, t[i].y -  r.y) );
        tRectInt.push_back( Point(t[i].x - r.x, t[i].y - r.y) ); // for fillConvexPoly
        
        t1Rect.push_back( Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
        t2Rect.push_back( Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
    }
    
    // Get mask by filling triangle
    Mat mask = Mat::zeros(r.height, r.width, CV_32FC3);
    fillConvexPoly(mask, tRectInt, Scalar(1.0, 1.0, 1.0), 16, 0);
    
    // Apply warpImage to small rectangular patches
    Mat img1Rect, img2Rect;
    img1(r1).copyTo(img1Rect);
    img2(r2).copyTo(img2Rect);
    
    Mat warpImage1 = Mat::zeros(r.height, r.width, img1Rect.type());
    Mat warpImage2 = Mat::zeros(r.height, r.width, img2Rect.type());
    
    applyAffineTransform(warpImage1, img1Rect, t1Rect, tRect);
    applyAffineTransform(warpImage2, img2Rect, t2Rect, tRect);
    
    // Alpha blend rectangular patches
    Mat imgRect = (1.0 - alpha) * warpImage1 + alpha * warpImage2;
    
    // Copy triangular region of the rectangular patch to the output image
    multiply(imgRect,mask, imgRect);
    multiply(img(r), Scalar(1.0,1.0,1.0) - mask, img(r));
    img(r) = img(r) + imgRect;
    
    
}

// triangularization
void calculateTriangles(vector<Triangle> & triangles)
{
    ifstream ifs(TRIANGLE_FILENAME);
    int x,y,z;
    while(ifs >> x >> y >> z)
    {
        Triangle t = {x, y, z};

        triangles.push_back(t);
    }

    if(triangles.size()!=0) // if points were already calculated, just load them
    return;


    // otherwise
    throw("Triangle file not found");
}



int main( int argc, char** argv)
{
    
    string filename1("../hillary_clinton.jpg");
    string filename2("../donald_trump.jpg");


    //alpha controls the degree of morph
    double alpha = 0.5;
    
    //Read input images
    Mat img1 = imread(filename1);
    Mat img2 = imread(filename2);
    
    //convert Mat to float data type
    img1.convertTo(img1, CV_32F);
    img2.convertTo(img2, CV_32F);
    
    
    //empty average image
    Mat imgMorph = Mat::zeros(img1.size(), CV_32FC3);
    

    //Read points
    vector<Point2f> points1, points2;
    calculateLandmarks(filename1, points1);
    calculateLandmarks(filename2, points2);

    vector<Point2f> points;

    cout<<points1.size();
    cout<<points2.size();


    //compute weighted average point coordinates
    for(int i = 0; i < points1.size(); i++)
    {
        float x1 = points1[i].x, x2 = points2[i].x , y1 = points1[i].y, y2 = points2[i].y;
        cout<<x1<<x2<<y1<<y2<<endl;
    }

    for(int i = 0; i < points1.size(); i++)
    {
        float x, y;
        float x1 = points1[i].x, x2 = points2[i].x , y1 = points1[i].y, y2 = points2[i].y;
        x = (1 - alpha)*x1 + alpha*x2;
        y =  ( 1 - alpha )*y1+ alpha*y2;
        
        points.push_back(Point2f(x,y));
        
    }

    vector<Triangle> triangles;
    //calculate triangle indices
    calculateTriangles(triangles);


    for(Triangle & tri : triangles)
    {
        int x = tri[0], y = tri[1], z=tri[2];

        // Triangles
        vector<Point2f> t1, t2, t;
        
        // Triangle corners for image 1.
        t1.push_back( points1[x] );
        t1.push_back( points1[y] );
        t1.push_back( points1[z] );
        
        // Triangle corners for image 2.
        t2.push_back( points2[x] );
        t2.push_back( points2[y] );
        t2.push_back( points2[z] );
        
        // Triangle corners for morphed image.
        t.push_back( points[x] );
        t.push_back( points[y] );
        t.push_back( points[z] );
        
        morphTriangle(img1, img2, imgMorph, t1, t2, t, alpha);
        
    }

    // Display Result
    imshow("Morphed Face", imgMorph / 255.0);
    waitKey(0);

    return 0;
}