#ifndef ADD_BEARD_H
#define ADD_BEARD_H

#include "../BaseFeature.h"
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

class AddBeard : public BaseFeature {
public:
    AddBeard(int argc, char** argv);
    ~AddBeard();
    void calculateAndDisplay ();
protected:
    cv::Mat beard_image; // image whose beard we will extract
    string beard_resized_name, beard_name, image_name;
    const double alpha = 1;
    cv::Point createPoint(dlib::full_object_detection &coord, int index);
    void manualTriangulation(cv::Mat &image, std::string filename, std::vector<dlib::full_object_detection> &landmarks,
                          std::vector<std::vector<int>> &triangulation_indices);
    void triangulationPoints(std::vector<dlib::full_object_detection> &landmarks, std::vector<std::vector<int>> &indices,
                          std::vector<std::vector<cv::Point2f>> &points);
    void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri,
                          std::vector<cv::Point2f> &dstTri);
    void warpTriangle(cv::Mat &img1, cv::Mat &img2, cv::Mat &img, std::vector<cv::Point2f> &t1,
                  std::vector<cv::Point2f> &t2, std::vector<cv::Point2f> &t, double alpha);
    void beardMask(dlib::full_object_detection &landmarks, std::vector<cv::Point2f> &mask);
};

#endif
