#include "processing.h"

#include <cereal/archives/binary.hpp>

#include "User.h"

processing::processing(const std::shared_ptr<cpptoml::table> &settings) {
    this->settings = settings;

    video_capture = new cv::VideoCapture(2, cv::CAP_V4L);

    std::thread recording_thread(&processing::start_camera_recording, this);

    std::thread processing_thread(&processing::start_image_process, this);

    std::thread face_recognition(&processing::start_face_recognition, this);

    std::this_thread::sleep_for(std::chrono::minutes(60));

    recording_thread.join();
    processing_thread.join();
    face_recognition.join();
}

void processing::start_camera_recording() {


    while (!stop_processing) {


        snapshot s;
        *video_capture >> s;

        video.enqueue(s);

        s = video.wait_dequeue();


    }

    // std::this_thread::sleep_for(std::chrono::seconds (40));
}

void processing::start_image_process() {

    Darkness *darkness;

    darkness = new Darkness(settings->get_as<double>("dark_threshold").value_or(50));

    while (!stop_processing) {
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

    dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();

    dlib::shape_predictor shapePredictor;
    dlib::deserialize("/etc/linux-hello/data/shape_predictor_5_face_landmarks.dat") >> shapePredictor;

    face_recognition_model_v1 faceRecognitionModelV1("/etc/linux-hello/data/dlib_face_recognition_resnet_model_v1.dat");

    double certainty_threshold = settings->get_as<double>("certainty_threshold").value_or(0.45);

    while (!stop_processing) {
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

                    stop_processing = true;
                }
            }
        }
    }
}
