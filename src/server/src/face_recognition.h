#ifndef LINUXHELLO_FACE_RECOGNITION_H
#define LINUXHELLO_FACE_RECOGNITION_H

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv/cv_image.h>
#include <security/pam_modules.h>
#include <unistd.h>

#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

#include "../../../include/cpptoml.h"
#include "darkness.h"
#include "face_recognition_model_v1.h"
#include "snapshot.h"

class face_recognition {
    cv::VideoCapture *capture;
    Darkness *darkness;

    std::shared_ptr<cpptoml::table> settings;

    std::vector<snapshot> snapshots;

    dlib::frontal_face_detector faceDetector;
    dlib::shape_predictor shapePredictor;
    face_recognition_model_v1 *faceRecognitionModelV1;

    bool continue_camera_record{};

    static double euclidean_distance(dlib::matrix<double> matrix);

   public:
    explicit face_recognition(const std::shared_ptr<cpptoml::table> &settings);

    int add(const std::string &username);

    int compare(const std::string &username);

    void camera_record();

    void test();
};

#endif  // LINUXHELLO_FACE_RECOGNITION_H
