#include <iostream>
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <opencv2/imgproc.hpp>
#include <unordered_map>
#include <opencv/cv.hpp>
#include <dlib/image_io.h>

// Definitions

// Create cv::Point from a landmark vector in the position index
cv::Point create_point(dlib::full_object_detection &coord, int index){
    return cv::Point( coord.part(index).x(), coord.part(index).y() );
}

// extract points of the right side of the beard
std::vector<cv::Point> extract_beard_Right(dlib::full_object_detection &landmarks){

    std::vector<cv::Point> beard;
    for(int i=8; i<=15; i++){
        beard.push_back( create_point(landmarks, i) );
    }
    for(int i=54; i<=57; i++){
        beard.push_back( create_point(landmarks, i) );
    }
    beard.push_back( create_point(landmarks, 8));
    return beard;
}

// extract points of the left side of the beard
std::vector<cv::Point> extract_beard_Left(dlib::full_object_detection &landmarks){

    std::vector<cv::Point> beard;
    for(int i=1; i<=8; i++){
        beard.push_back( create_point(landmarks, i) );
    }
    for(int i=57; i<=59; i++){
        beard.push_back( create_point(landmarks, i) );
    }
    beard.push_back( create_point(landmarks, 48));
    beard.push_back((create_point(landmarks, 1)));
    return beard;
}

// Draw delaunay triangles
static std::vector<cv::Vec6f> draw_delaunay( cv::Mat &img, cv::Subdiv2D &subdiv, dlib::full_object_detection &landmarks)
{
    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    std::vector<cv::Point> pt(3);
    cv::Size size = img.size();
    cv::Rect rect(0,0, size.width, size.height);

    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        cv::Vec6f t = triangleList[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

        // Draw rectangles completely inside the image.
        if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
        {
            if( pt[0].x == landmarks.part(0).x() && pt[0].y == landmarks.part(0).y() ||
                    pt[1].x == landmarks.part(0).x() && pt[1].y == landmarks.part(0).y() ||
                    pt[2].x == landmarks.part(0).x() && pt[2].y == landmarks.part(0).y() ) {
                continue;
            }
            if( pt[0].x == landmarks.part(16).x() && pt[0].y == landmarks.part(16).y() ||
                    pt[1].x == landmarks.part(16).x() && pt[1].y == landmarks.part(16).y() ||
                    pt[2].x == landmarks.part(16).x() && pt[2].y == landmarks.part(16).y() ){
                continue;
            }

            line(img, pt[0], pt[1], cv::Scalar(255, 0, 0), 1, CV_AA, 0);
            line(img, pt[1], pt[2], cv::Scalar(255, 0, 0), 1, CV_AA, 0);
            line(img, pt[2], pt[0], cv::Scalar(255, 0, 0), 1, CV_AA, 0);
        }
    }

    return triangleList;
}

// Triangulation in the beard region
void Triangulation(cv::Mat &image, std::vector<dlib::full_object_detection> &landmarks, std::vector<cv::Vec6f> &triangularLeft,
                   std::vector<cv::Vec6f> &triangularRight){

    // Load face detection and pose estimation models
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor pose_model;
    dlib::deserialize("../shape_predictor_68_face_landmarks.dat") >> pose_model;

    // Creating a object from Mat into something dlib can deal with
    dlib::cv_image<dlib::bgr_pixel> cimg(image);

    // Detect faces
    std::vector<dlib::rectangle> faces = detector(cimg, 1);

    // Find the pose of each face
    std::vector<dlib::full_object_detection> shapes;
    for(unsigned long i=0; i < faces.size(); ++i){
        shapes.push_back(pose_model(cimg, faces[i]));
    }

    int k = 0; // we have only one image

    // TRIANGULATION PART

    // Auxiliaries points to construct the triangulation
    std::vector<cv::Point> right_points = extract_beard_Right(shapes[k]);
    std::vector<cv::Point> left_points = extract_beard_Left(shapes[k]);

    // Bounded region to triangulation
    cv::Rect bounded_region(0, 0, image.size().width, image.size().height);

    // Triangulation of the right side
    cv::Subdiv2D subdiv1(bounded_region);
    for(cv::Point2f p : right_points){
        subdiv1.insert(p);
    }

    triangularRight = draw_delaunay(image, subdiv1, shapes[k]);

    // Triangulation of the left side
    cv::Subdiv2D subdiv2(bounded_region);
    for(cv::Point2f p : left_points){
        subdiv2.insert(p);
    }

    triangularLeft = draw_delaunay(image, subdiv2, shapes[k]);

    // Verifying if all the points are inside the image
    std::vector<cv::Point> points(3);
    for(int i=0; i < triangularLeft.size(); i++){

        // Test TriangularLeft
        cv::Vec6f t = triangularLeft[i];
        points[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        points[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        points[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

        if ( bounded_region.contains(points[0]) && bounded_region.contains(points[1]) && bounded_region.contains(points[2])){
            // all good
        }
        else{
            triangularLeft.erase(triangularLeft.begin()+i);
            std::cout << "apagando ponto Left" << std::endl;
        }

        // Test TriangularRight
        t = triangularRight[i];
        points[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        points[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        points[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

        if ( bounded_region.contains(points[0]) && bounded_region.contains(points[1]) && bounded_region.contains(points[2])){
            // all good
        }
        else{
            triangularRight.erase(triangularRight.begin()+i);
            std::cout << "apagando ponto Right" << std::endl;
        }

    }

}

void Manual_Triangulation(cv::Mat &image, std::string filename, std::vector<dlib::full_object_detection> &landmarks, std::vector<std::vector<int>> &triangulation_indices){

    // Load face detection and pose estimation models
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor pose_model;
    dlib::deserialize("../shape_predictor_68_face_landmarks.dat") >> pose_model;

    // Creating a object from Mat into something dlib can deal with
    dlib::array2d<dlib::rgb_pixel> cimg;
    dlib::load_image(cimg, filename);

    // Detect faces
    std::vector<dlib::rectangle> faces = detector(cimg, 1);

    // Find the pose of each face
    for(unsigned long i=0; i < faces.size(); ++i){
        landmarks.push_back(pose_model(cimg, faces[i]));
    }

    int k = 0; // we have only one image

    std::string points_file = "../points.txt";
    std::ifstream ifs (points_file.c_str());
    int a, b, c;

    while (ifs >> a >> b >> c) {
        triangulation_indices.push_back( {a, b, c} );
    }

}

void triangulation_points(std::vector<dlib::full_object_detection> &landmarks, std::vector<std::vector<int>> &indices,
                          std::vector<std::vector<cv::Point2f>> &points){

    for(std::vector<int> v : indices){
        points.push_back( { create_point(landmarks[0], v[0]), create_point(landmarks[0], v[1]), create_point(landmarks[0], v[2])  } );
    }
}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri, std::vector<cv::Point2f> &dstTri)
{
    // Given a pair of triangles, find the affine transform.
    cv::Mat warpMat = cv::getAffineTransform( srcTri, dstTri );

    // Apply the Affine Transform just found to the src image
    cv::warpAffine( src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(cv::Mat &img1, cv::Mat &img2, cv::Mat &img, std::vector<cv::Point2f> &t1, std::vector<cv::Point2f> &t2,
                  std::vector<cv::Point2f> &t, double alpha) {

    // Find bounding rectangle for each triangle
    cv::Rect r = boundingRect(t);
    cv::Rect r1 = boundingRect(t1);
    cv::Rect r2 = boundingRect(t2);

    // Offset points by left top corner of the respective rectangles
    std::vector<cv::Point2f> t1Rect, t2Rect, tRect;
    std::vector<cv::Point> tRectInt;
    for(int i = 0; i < 3; i++)
    {
        tRect.push_back( cv::Point2f( t[i].x - r.x, t[i].y - r.y) );
        tRectInt.push_back( cv::Point(t[i].x - r.x, t[i].y - r.y) ); // for fillConvexPoly

        t1Rect.push_back( cv::Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
        t2Rect.push_back( cv::Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
    }

    // Get mask by filling triangle
    cv::Mat mask = cv::Mat::zeros(r.height, r.width, CV_32FC3);
    fillConvexPoly(mask, tRectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

    // Apply warpImage to small rectangular patches
    cv::Mat img1Rect, img2Rect;
    img1(r1).copyTo(img1Rect);
    img2(r2).copyTo(img2Rect);

    cv::Mat warpImage1 = cv::Mat::zeros(r.height, r.width, img1Rect.type());
    cv::Mat warpImage2 = cv::Mat::zeros(r.height, r.width, img2Rect.type());

    applyAffineTransform(warpImage1, img1Rect, t1Rect, tRect);
    applyAffineTransform(warpImage2, img2Rect, t2Rect, tRect);

    //Alpha blend rectangular patches

    cv::Mat imgRect = (1.0 - alpha) * warpImage1 + alpha * warpImage2;

    // Copy triangular region of the rectangular patch to the output image
    multiply(imgRect,mask, imgRect);
    multiply(img(r), cv::Scalar(1.0,1.0,1.0) - mask, img(r));
    img(r) = img(r) + imgRect;

}

void beard_mask(dlib::full_object_detection &landmarks, std::vector<cv::Point2f> &mask){

    for(int i=1; i<=15; i++){
        mask.push_back(create_point(landmarks, i));
    }

    for(int i=54; i <= 59; i++){
        mask.push_back(create_point(landmarks, i));
    }

    mask.push_back(create_point(landmarks, 48));
    mask.push_back(create_point(landmarks, 1));

}

int main() {


    // Loading images
    std::string image_name = "../tomcruise2.jpg";
    std::string beard_name = "../whitebeard_size.jpg";

    cv::Mat image = cv::imread(image_name);
    cv::Mat beard_image = cv::imread(beard_name);

//    if(image.size().width != beard_image.size().width ||
//            image.size().height != beard_image.size().height){
//        std::cout << "doing resize" << std::endl;
//        cv::resize(beard_image, beard_image, image.size(), 0, 0, cv::INTER_LINEAR);
////        cv::resize(image, image, beard_image.size(), 0, 0, cv::INTER_LINEAR);
//    }

    // Resize
//    cv::resize(beard_image, beard_image, image.size(), 0, 0, cv::INTER_LINEAR);
////    cv::resize(image, image, beard_image.size(), 0, 0, cv::INTER_LINEAR);
//    cv::imwrite("../whitebeard_size.jpg", beard_image);


    // Convert Mat to float data type
    image.convertTo(image, CV_32F);
    beard_image.convertTo(beard_image, CV_32F);

    // Clone original image
    cv::Mat warpedImage = image.clone();

    // Display both images before the morphing
//    cv::namedWindow("face", 1);
//    imshow("face", image/255.0);
//    cv::namedWindow("beard_face", 1);
//    imshow("beard_face", beard_image/255.0);
//
//    cv::waitKey(0);


    // Triangulation Algorithm
    std::vector<dlib::full_object_detection> Image_landmarks;
    std::vector< std::vector<int> > Image_triangulation_indices;
//    std::vector<cv::Vec6f> triangularLeft, triangularRight;
//    Triangulation(image, Image_landmarks, triangularLeft, triangularRight);

    std::vector<dlib::full_object_detection> BeardImage_landmarks;
    std::vector< std::vector<int> > BeardImage_triangulation_indices;
//    std::vector<cv::Vec6f> triangularRight_beard_image, triangularLeft_beard_image;
//    Triangulation(beard_image, BeardImage_landmarks, triangularLeft_beard_image, triangularRight_beard_image);

    Manual_Triangulation(image, image_name, Image_landmarks, Image_triangulation_indices);
    Manual_Triangulation(beard_image, beard_name, BeardImage_landmarks, BeardImage_triangulation_indices);

    std::vector<std::vector<cv::Point2f>> Image_triangulation_points;
    std::vector<std::vector<cv::Point2f>> BeardImage_triangulation_points;

    triangulation_points(Image_landmarks, Image_triangulation_indices, Image_triangulation_points);
    triangulation_points(BeardImage_landmarks, BeardImage_triangulation_indices, BeardImage_triangulation_points);

    // ALPHA
    double alpha = 0.2;

    // Type of Morphing
    bool full = true;

    // WarpTriangles
    int size = Image_triangulation_points.size();

    if(!full){
        for(int i=0; i < size; i++){

            if(i==0 || i == size - 1) alpha = 0.20;
            else{
                if(i==1 || i == size - 2) alpha = 0.25;
                else alpha = 0.3;
            }

            warpTriangle(image, beard_image, warpedImage, Image_triangulation_points[i], BeardImage_triangulation_points[i],
                         Image_triangulation_points[i], alpha);
        }
    }
    else{
        for(int i=0; i < size; i++) {
            warpTriangle(image, beard_image, warpedImage, Image_triangulation_points[i], BeardImage_triangulation_points[i],
                        Image_triangulation_points[i], 1);
        }
    }

    cv::imwrite("../morphedfull7.jpg", warpedImage);

    // Create an all white mask
    std::vector<cv::Point2f> mask;
    beard_mask(Image_landmarks[0], mask);
    cv::Mat src_mask = cv::Mat::zeros(image.rows, image.cols, image.depth());
    std::vector<cv::Point> t;
    for(std::vector<cv::Point2f> triangle : Image_triangulation_points){
        t.clear();
        for(int i=0; i<3; i++){
            t.push_back(cv::Point(cvRound(triangle[i].x), cvRound(triangle[i].y)));
        }
        cv::fillConvexPoly(src_mask, t, cv::Scalar(255,255,255), 16, 0);
    }

    cv::imwrite("../mask7.jpg", src_mask);

}