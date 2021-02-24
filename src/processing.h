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

   public:

    explicit processing(const std::shared_ptr<cpptoml::table> &settings);

};

#endif  // LINUXHELLO_PROCESSING_H
