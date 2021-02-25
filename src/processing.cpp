#include "processing.h"

#include <cereal/archives/binary.hpp>

#include "User.h"

processing::processing(const std::shared_ptr<cpptoml::table> &settings) {
    this->settings = settings;

    video_capture = new cv::VideoCapture(2, cv::CAP_V4L);

    std::thread recording_thread(&processing::start_camera_recording, this);

    std::thread processing_thread(&processing::start_image_process, this);

    faceDetector = dlib::get_frontal_face_detector();

    dlib::deserialize("/etc/linux-hello/data/shape_predictor_5_face_landmarks.dat") >> shapePredictor;

    faceRecognitionModelV1 =
        face_recognition_model_v1("/etc/linux-hello/data/dlib_face_recognition_resnet_model_v1.dat");

    std::thread face_recognition(&processing::start_face_recognition, this);

    sched_param sch_params{};

    set_priority(recording_thread, 3);
    set_priority(processing_thread, 4);
    set_priority(face_recognition, 5);

    int test = SCHED_RR;

    pthread_getschedparam(recording_thread.native_handle(), &test, &sch_params);

    std::cout << "Thread prior" << sch_params.sched_priority << std::endl;

    while (!stop_processing) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Video size: " << video.size() << " Processing: " << converted.size()
                  << " Recognized: " << recognized.size() << std::endl;
    }

    recording_thread.join();
    processing_thread.join();
    face_recognition.join();
}

void processing::start_camera_recording() {
    while (!stop_processing) {
        // std::cout << "test" << std::endl;

        snapshot s;
        *video_capture >> s;

        video.enqueue(s);

        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // std::this_thread::sleep_for(std::chrono::seconds (40));
}

void processing::start_image_process() {
    // std::cout << "tes5" << std::endl;

    Darkness *darkness;

    darkness = new Darkness(settings->get_as<double>("dark_threshold").value_or(50));

    while (!stop_processing) {
        // std::cout << "test2" << std::endl;

        snapshot s = video.wait_dequeue();

        if (!darkness->darkness_check(s)) continue;

        s.convert_image();

        converted.enqueue(s);
    }
}

void processing::start_face_recognition() {
    std::string username = "denis";

    std::string model_name = "/etc/linux-hello/models/" + username + ".dat";
    std::ifstream input(model_name);
    User user;

    if (input.good()) {
        cereal::BinaryInputArchive cereal_input(input);
        cereal_input(CEREAL_NVP(user));
    }

    double certainty_threshold = settings->get_as<double>("certainty_threshold").value_or(0.45);

    while (!stop_processing) {
        std::cout << "test3" << std::endl;

        std::cout << "Recognition" << std::endl;

        snapshot s = converted.wait_dequeue();

        s.face_locations = faceDetector(s.dlib_image);

        for (auto &fl : s.face_locations) {
            s.face_landmark = shapePredictor(s.dlib_image, fl);
            s.face_encoding = faceRecognitionModelV1.compute_face_descriptor(
                s.dlib_image, s.face_landmark, settings->get_as<double>("compare_num_jitters").value_or(1));
            for (auto &enc : user.user_encodings) {
                double certainty = face_recognition::euclidean_distance(enc.encodings[0].data - s.face_encoding);

                if (certainty_threshold > certainty) {
                    std::cout << "Fuck it worked" << std::endl;

                    recognized.enqueue(s);
                }
            }
        }
    }
}
void processing::set_priority(std::thread &th, int priority) {
    sched_param sch_param{.sched_priority = priority};

    pthread_setschedparam(th.native_handle(), SCHED_RR, &sch_param);
}
