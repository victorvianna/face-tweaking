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

int main(){
    // Loading images
    std::string image_name = "../tomcruise2.jpg";
    std::string warped_name = "../morphedfull7.jpg";
    std::string mask_name = "../mask7.jpg";

    cv::Mat image = cv::imread(image_name);
    cv::Mat warped_image = cv::imread(warped_name);
    cv::Mat mask = cv::imread(mask_name, 0);

    cv::Rect r = cv::boundingRect(mask);

    cv::Point center = (r.tl() + r.br()) / 2;

//    cv::Point center;
//    center.x = image.size().width/2  - 35;
//    center.y = image.size().height/2 + 120;

    // Seamlessly clone src into dst and put the results in output
    cv::Mat normal_clone;
    cv::Mat mixed_clone;

//    cvtColor(warped_image, warped_image, CV_RGBA2RGB, 0);
//    cvtColor(image, image, CV_RGBA2RGB, 0);
//
//    warped_image.convertTo(warped_image, CV_8UC3, 1, 0);
//    image.convertTo(image, CV_8UC3, 1, 0);



    cv::seamlessClone(warped_image, image, mask, center, normal_clone, cv::NORMAL_CLONE);
    cv::seamlessClone(warped_image, image, mask, center, mixed_clone, cv::MIXED_CLONE);

    cv::imwrite("../morphed_normalclone7.jpg", normal_clone);
    cv::imwrite("../morphed_mixedclone7.jpg", mixed_clone);

    std::cout<< "checkout" << std::endl;

    cv::waitKey(0);
}