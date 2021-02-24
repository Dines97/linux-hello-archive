#include "camera.h"

camera::camera() { capture = new cv::VideoCapture(cv::CAP_V4L); }

camera& operator>>(camera &c, snapshot& s) {
    *c.capture >> s;
    return c;
}
