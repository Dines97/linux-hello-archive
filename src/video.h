#ifndef LINUXHELLO_VIDEO_H
#define LINUXHELLO_VIDEO_H

#include <vector>

#include "snapshot.h"

#include "synchronized_queue.h"
class video {

   public:
    synchronized_queue<snapshot> video_record;


    void add_snapshot(snapshot &s);


};

#endif  // LINUXHELLO_VIDEO_H
