#ifndef LINUXHELLO_RECORDING_H
#define LINUXHELLO_RECORDING_H

#include "camera.h"
#include "video.h"
class recording {

   public:

    camera camera;
    video video;
    bool stop_recording = false;


    recording();
    void loop();
};

#endif  // LINUXHELLO_RECORDING_H
