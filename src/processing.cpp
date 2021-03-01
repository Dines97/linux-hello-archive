#include "processing.h"

#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/render_face_detections.h>

#include <cereal/archives/binary.hpp>
#include <utility>

processing::processing(const std::shared_ptr<cpptoml::table> &settings, std::string user_name) {
    this->settings = settings;
    this->user_name = std::move(user_name);

    std::thread recording_thread(&processing::start_camera_recording, this);

    std::thread processing_thread(&processing::start_image_process, this);

    faceDetector = dlib::get_frontal_face_detector();

    std::thread face_detection(&processing::start_face_detection, this);

    dlib::deserialize("/etc/linux-hello/data/shape_predictor_68_face_landmarks.dat") >> shapePredictor;

    faceRecognitionModelV1 =
        new face_recognition_model_v1("/etc/linux-hello/data/dlib_face_recognition_resnet_model_v1.dat");

    std::string model_name = "/etc/linux-hello/models/" + this->user_name + ".dat";
    std::ifstream input(model_name);

    if (input.good()) {
        cereal::BinaryInputArchive cereal_input(input);
        cereal_input(CEREAL_NVP(user));
    }

    std::thread face_recognition1(&processing::start_face_recognition, this);
    std::thread face_recognition2(&processing::start_face_recognition, this);
    std::thread face_recognition3(&processing::start_face_recognition, this);
    std::thread face_recognition4(&processing::start_face_recognition, this);
    std::thread face_recognition5(&processing::start_face_recognition, this);
    std::thread face_recognition6(&processing::start_face_recognition, this);

    std::thread test(&processing::test, this);

    while (!stop_processing) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Video size: " << video.size() << " Processing: " << converted.size()
                  << " Detected: " << detected.size() << " Recognized: " << recognized.size() << std::endl;
    }

    recording_thread.join();
    processing_thread.join();
    face_detection.join();
    face_recognition1.join();
    face_recognition2.join();
    face_recognition3.join();
    face_recognition4.join();
    face_recognition5.join();
    face_recognition6.join();
    test.join();
}

void processing::start_camera_recording() {
    cv::VideoCapture *video_capture;

    if (settings->get_as<bool>("auto_find_camera").value_or(true)) {
        video_capture = new cv::VideoCapture(cv::CAP_V4L);
    } else {
        video_capture = new cv::VideoCapture(settings->get_as<int>("camera_index").value_or(0), cv::CAP_V4L);
    }

    if (!video_capture->isOpened()) {
        std::cout << "Couldn't open camera. Aborting" << std::endl;
        abort();
    }

    while (!stop_processing) {
        auto *s = new snapshot;
        *video_capture >> *s;

        video.enqueue(s);
    }
}

void processing::start_image_process() {

    auto *darkness = new Darkness(settings->get_as<double>("dark_threshold").value_or(50));

    while (!stop_processing) {
        snapshot *s = video.wait_dequeue();

        if (!darkness->darkness_check(*s)) continue;

        s->convert_image();

        converted.enqueue(s);
    }
}

void processing::start_face_detection() {
    while (!stop_processing) {
        snapshot *s = converted.wait_dequeue();
        s->face_locations = faceDetector(s->dlib_image);
        detected.enqueue(s);
    }
}

void processing::start_face_recognition() {

    while (!stop_processing) {
        snapshot *s = detected.wait_dequeue();

        for (auto &fl : s->face_locations) {
            s->face_landmark = shapePredictor(s->dlib_image, fl);
            s->face_encoding = faceRecognitionModelV1->compute_face_descriptor(
                s->dlib_image, s->face_landmark, settings->get_as<double>("compare_num_jitters").value_or(1));

            recognized.enqueue(s);
        }
    }
}

void processing::test() {
    dlib::image_window win;

    while (!stop_processing) {
        snapshot *s = debug.wait_dequeue();

        win.clear_overlay();
        win.set_image(s->dlib_image);

        if (s->face_landmark.num_parts() == 5 || s->face_landmark.num_parts() == 68)
            win.add_overlay(render_face_detections(s->face_landmark));

        win.add_overlay(s->face_locations, dlib::rgb_pixel(255, 0, 0));
    }
}

int processing::get_status() {
    std::unique_lock<std::mutex> lock(status_mutex);

    status_cv.wait(lock, [this] { return this->status_ready; });

    return status;
}

double processing::euclidean_distance(dlib::matrix<double> matrix) {
    double sum = 0;

    for (int i = 0; i < matrix.nr(); i++) {
        sum += pow(matrix(i, 0), 2);
    }

    return sqrt(sum);
}