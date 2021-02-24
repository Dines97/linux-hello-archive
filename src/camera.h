#ifndef LINUXHELLO_CAMERA_H
#define LINUXHELLO_CAMERA_H

#include <opencv2/opencv.hpp>

#include "snapshot.h"

class camera {
    cv::VideoCapture* capture;

   public:
    camera();

    friend camera& operator>>(camera& camera, snapshot& s);
};

#endif  // LINUXHELLO_CAMERA_H
