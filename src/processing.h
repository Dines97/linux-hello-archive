#ifndef LINUXHELLO_PROCESSING_H
#define LINUXHELLO_PROCESSING_H

#include "darkness.h"
#include "face_recognition.h"
#include "synchronized_queue.h"

class processing {

    cv::VideoCapture *video_capture;

    bool stop_processing = false;

    //std::string &username;

    std::shared_ptr<cpptoml::table> settings;

    synchronized_queue<snapshot> video;
    synchronized_queue<snapshot> converted;
    synchronized_queue<snapshot> recognized;

    void start_camera_recording();

    void start_image_process();

    void start_face_recognition();

    dlib::frontal_face_detector faceDetector;
    dlib::shape_predictor shapePredictor;
    face_recognition_model_v1 faceRecognitionModelV1;


   public:

    explicit processing(const std::shared_ptr<cpptoml::table> &settings);

    static void set_priority(std::thread &th, int priority);

};

#endif  // LINUXHELLO_PROCESSING_H
