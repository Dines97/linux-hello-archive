#include "face_recognition.h"

#include "thread"

// TODO: Rethink variable name in all project especially in this file
face_recognition::face_recognition(const std::shared_ptr<cpptoml::table> &settings, const std::string &user_name) {
    this->settings = settings;
    this->user_name = user_name;

    main_processing = new processing(settings, user_name);
}

int face_recognition::add(const std::string &username) {}

int face_recognition::compare(const std::string &username) {}

void face_recognition::test() { main_processing->test(); }
