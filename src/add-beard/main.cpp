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

// Get triangle triples from file 'points.txt' and find the face landmarks
void Manual_Triangulation(cv::Mat &image, std::string filename, std::vector<dlib::full_object_detection> &landmarks,
                          std::vector<std::vector<int>> &triangulation_indices){

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
void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri,
                          std::vector<cv::Point2f> &dstTri) {
    // Given a pair of triangles, find the affine transform.
    cv::Mat warpMat = cv::getAffineTransform( srcTri, dstTri );

    // Apply the Affine Transform just found to the src image
    cv::warpAffine( src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(cv::Mat &img1, cv::Mat &img2, cv::Mat &img, std::vector<cv::Point2f> &t1,
                  std::vector<cv::Point2f> &t2, std::vector<cv::Point2f> &t, double alpha) {

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
    std::string beard_name = "../whitebeard.jpg";
//    std::string beard_name = "../whitebeard_resized.jpg";
    std::string beard_resized_name = "../whitebeard_resized.jpg";
    std::string full_morphed_name = "../fullmorphed.jpg";
    std::string mask_name = "../mask.jpg";

    cv::Mat image = cv::imread(image_name);
    cv::Mat beard_image = cv::imread(beard_name);
    cv::Mat original_beard_image = beard_image.clone();

    if(image.size().width != original_beard_image.size().width ||
            image.size().height != original_beard_image.size().height){
        std::cout << "Doing Resize" << std::endl;
        cv::resize(original_beard_image, beard_image, image.size(), 0, 0, cv::INTER_LINEAR);
        cv::imwrite(beard_resized_name, beard_image);
        beard_image = cv::imread(beard_resized_name);
    }

    std::cout << beard_image.size() << std::endl;
    std::cout << original_beard_image.size() << std::endl;
    std::cout << image.size() << std::endl;

    return 0;

    // Convert Mat to float data type
    image.convertTo(image, CV_32F);
    beard_image.convertTo(beard_image, CV_32F);

    // Clone original image
    cv::Mat warpedImage = image.clone();

    // Triangulation Algorithm
    std::cout << "Triangulation Algorithm" << std::endl;
    std::vector<dlib::full_object_detection> Image_landmarks, BeardImage_landmarks;
    std::vector< std::vector<int> > Image_triangulation_indices, BeardImage_triangulation_indices;

    Manual_Triangulation(image, image_name, Image_landmarks, Image_triangulation_indices);
    Manual_Triangulation(beard_image, beard_name, BeardImage_landmarks, BeardImage_triangulation_indices);

    std::vector<std::vector<cv::Point2f>> Image_triangulation_points, BeardImage_triangulation_points;

    triangulation_points(Image_landmarks, Image_triangulation_indices, Image_triangulation_points);
    triangulation_points(BeardImage_landmarks, BeardImage_triangulation_indices, BeardImage_triangulation_points);

    // ALPHA
    double alpha = 1;

    // WarpTriangles
    std::cout << "WarpTriangles" << std::endl;
    int size = Image_triangulation_points.size();
    for(int i=0; i < size; i++) {
        warpTriangle(image, beard_image, warpedImage, Image_triangulation_points[i],
                     BeardImage_triangulation_points[i], Image_triangulation_points[i], alpha);
    }

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

    // Saving FullMorphed and Mask
    cv::imwrite(full_morphed_name, warpedImage);
    cv::imwrite(mask_name, src_mask);
    cv::Mat mask_image = cv::imread(mask_name, 0);

    // Loading sourceImage and warpedImage
    cv::Mat original_image = cv::imread(image_name);
    cv::Mat warped_image = cv::imread(full_morphed_name);

    // Finding position to seamlessclone function
    cv::Rect r = cv::boundingRect(mask);
    cv::Point center = (r.tl() + r.br()) / 2;

    // Seamlessly clone src into dst and put the results in output
    cv::Mat normal_clone;
    cv::Mat mixed_clone;

    std::cout << "SeamLessClone Algorithm" << std::endl;
    cv::seamlessClone(warped_image, original_image, mask_image, center, normal_clone, cv::NORMAL_CLONE);
    cv::seamlessClone(warped_image, original_image, mask_image, center, mixed_clone, cv::MIXED_CLONE);

    cv::imwrite("../TESTmorphed_normalclone.jpg", normal_clone);
    cv::imwrite("../TESTmorphed_mixedclone.jpg", mixed_clone);

}