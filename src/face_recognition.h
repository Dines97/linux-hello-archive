#ifndef LINUXHELLO_FACE_RECOGNITION_H
#define LINUXHELLO_FACE_RECOGNITION_H

#include <dlib/opencv/cv_image.h>
#include <security/pam_modules.h>
#include <unistd.h>

#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

#include "../include/cpptoml.h"
#include "darkness.h"
#include "face_recognition_model_v1.h"
#include "processing.h"
#include "snapshot.h"

class face_recognition {
    std::shared_ptr<cpptoml::table> settings;
    std::string user_name;

    processing *main_processing;

   public:
    face_recognition(const std::shared_ptr<cpptoml::table> &settings, const std::string &);

    int add(const std::string &username);

    int compare(const std::string &username);

    void test();
};

#endif  // LINUXHELLO_FACE_RECOGNITION_H
