#ifndef LINUXHELLO_PROCESSING_H
#define LINUXHELLO_PROCESSING_H

#include <dlib/image_processing/frontal_face_detector.h>

#include "../include/cpptoml.h"
#include "User.h"
#include "darkness.h"
#include "face_recognition_model_v1.h"
#include "snapshot.h"
#include "synchronized_queue.h"

class processing {
    User user;

    std::mutex status_mutex;
    std::condition_variable status_cv;

    bool stop_processing = false;

    std::string user_name;

    std::shared_ptr<cpptoml::table> settings;

    synchronized_queue<snapshot *> debug;
    synchronized_queue<snapshot *> video;
    synchronized_queue<snapshot *> converted;
    synchronized_queue<snapshot *> detected;
    synchronized_queue<snapshot *> recognized;

    dlib::frontal_face_detector faceDetector;
    dlib::shape_predictor shapePredictor;
    face_recognition_model_v1 *faceRecognitionModelV1;

    void start_camera_recording();

    void start_image_process();

    void start_face_detection();

    void start_face_recognition();

    bool status_ready = false;
    int status = -1;

    static double euclidean_distance(dlib::matrix<double> matrix);

   public:
    processing(const std::shared_ptr<cpptoml::table> &settings, std::string);

    void test();

    int get_status();
};

#endif  // LINUXHELLO_PROCESSING_H
