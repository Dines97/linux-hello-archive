#include "recording.h"

recording::recording() {

    loop();

}
void recording::loop() {
    while (!stop_recording) {
        snapshot s;
        camera >> s;
        this->video.add_snapshot(s);
    }
}
