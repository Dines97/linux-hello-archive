#include "face_recognition.h"

#include <cereal/archives/binary.hpp>
#include <vector>

#include "User.h"
#include "timings.h"

// TODO: Rethink variable name in all project especially in this file
face_recognition::face_recognition(const std::shared_ptr<cpptoml::table> &settings)
    : faceRecognitionModelV1("/etc/linux-hello/data/dlib_face_recognition_resnet_model_v1.dat") {
    this->settings = settings;

    darkness = new Darkness(settings->get_as<double>("dark_threshold").value_or(50));

    dlib::deserialize("/etc/linux-hello/data/shape_predictor_5_face_landmarks.dat") >> shapePredictor;

    if (settings->get_as<bool>("auto_find_camera").value_or(true)) {
        capture = new cv::VideoCapture(cv::CAP_V4L);
    } else {
        capture = new cv::VideoCapture(settings->get_as<int>("camera_index").value_or(0), cv::CAP_V4L);
    }

    if (!capture->isOpened()) {
        std::cout << "Couldn't open camera. Aborting" << std::endl;
        abort();
    }
}

int face_recognition::add(const std::string &username) {
    std::string model_name = "/etc/linux-hello/models/" + username + ".dat";
    std::fstream f_models;
    f_models.open(model_name, std::ios::in);
    User user;

    if (f_models.good()) {
        cereal::BinaryInputArchive cereal_input(f_models);
        cereal_input(CEREAL_NVP(user));
    }

    std::string label;
    std::cout << "Enter a label for new model: ";
    std::cin >> label;

    int valid_encodings = 0;
    double timeout = settings->get_as<double>("add_timeout").value_or(3);
    bool face_recognized = false;
    dlib::matrix<double> total_face_encoding;

    std::streamsize pp = std::cout.precision();
    std::cout << std::setprecision(1) << "Please look straight into the camera for " << timeout << " seconds"
              << std::setprecision(pp) << std::endl;
    sleep((unsigned int)2);

    const auto time = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time).count() <
           timeout * 1000) {
        camera_record();
    }

    capture->release();
    std::cout << "Analyzing..." << std::endl;

    for (auto &s : snapshots) {
        if (s.opencv_image.empty()) {
            std::cout << "Empty" << std::endl;
        }

        if (!darkness->darkness_check(s)) continue;

        s.convert_image();

        s.face_locations = faceDetector(s.dlib_image);
        if (s.face_locations.empty()) continue;

        face_recognized = true;

        if (s.face_locations.size() > 1) {
            std::cerr << "Multiple faces detected, aborting";
            return 1;
        }

        s.face_location = s.face_locations[0];
        s.face_landmark = shapePredictor(s.dlib_image, s.face_location);
        s.face_encoding = faceRecognitionModelV1.compute_face_descriptor(s.dlib_image, s.face_landmark,
                                                                         settings->get_as<double>("add_num_jitters").value_or(1));

        total_face_encoding += s.face_encoding;
        valid_encodings++;
    }

    if (!face_recognized) {
        if (darkness->getValidFrames() == 0) {
            std::cout << "Camera saw only black frames - is IR emitter working?" << std::endl;
        } else if (darkness->getValidFrames() == darkness->getDarkTries()) {
            std::cout << "All frames were too dark, please check dark_threshold in config" << std::endl
                      << "Average darkness: " << darkness->getDarkRunningTotal() / darkness->getValidFrames()
                      << ", Threshold: " << darkness->getDarkThreshold();
        } else {
            std::cout << "No face detected, aborting" << std::endl;
        }
        return 1;
    }

    std::cout << "Saving..." << std::endl;

    total_face_encoding /= valid_encodings;

    Encoding encoding;
    encoding.camera_index = settings->get_as<int>("camera_index").value_or(0);
    encoding.data = total_face_encoding;

    UserEncoding user_encoding;
    user_encoding.unix_time = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    user_encoding.id = user.user_encodings.size();
    user_encoding.label = label;
    user_encoding.encodings.push_back(encoding);

    user.user_name = username;
    user.user_encodings.push_back(user_encoding);

    // TODO: Check is flush required if close called after it
    f_models.flush();
    f_models.close();
    f_models.clear();
    f_models.open(model_name, std::ios::out | std::ios::trunc);

    if (f_models.good()) {
        cereal::BinaryOutputArchive cereal_output(f_models);
        cereal_output(CEREAL_NVP(user));
    }

    f_models.flush();
    f_models.close();

    std::cout << "Scan complete. Added a new model to " << username << std::endl;

    return 0;
}

int face_recognition::compare(const std::string &username) {
    std::string model_name = "/etc/linux-hello/models/" + username + ".dat";
    std::ifstream input(model_name);
    User user;

    if (input.good()) {
        cereal::BinaryInputArchive cereal_input(input);
        cereal_input(CEREAL_NVP(user));
    }

    if (input.bad() || user.user_encodings.empty()) {
        std::cout << "No face model known" << std::endl
                  << "Please add face model with:" << std::endl
                  << "\tsudo linux-hello --add" << std::endl;
        return PAM_USER_UNKNOWN;
    }

    if (settings->get_as<double>("detection_notice").value_or(false)) {
        std::cout << "Attempting face detection" << std::flush;
    }

    double certainty_threshold = settings->get_as<double>("certainty_threshold").value_or(0.45);
    double timeout = settings->get_as<double>("timeout").value_or(1);

    snapshot s;

    const auto time = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time).count() <
           timeout * 1000) {
        *capture >> s;

        if (s.opencv_image.empty()) {
            std::cout << "Empty" << std::endl;
        }

        if (!darkness->darkness_check(s)) continue;

        s.convert_image();

        s.face_locations = faceDetector(s.dlib_image);

        if (!s.face_locations.empty()) break;
    }

    if (s.face_locations.empty() &&
        (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time).count() >
         timeout * 1000)) {
        if (settings->get_as<double>("detection_notice").value_or(false))
            std::cout << '\r' << "Exceeded the time limit for face recognition         " << std::endl;

        return PAM_AUTH_ERR;
    }

    double best_certainty;
    bool first = true;
    int best_certainty_index;

    for (auto &fl : s.face_locations) {
        s.face_landmark = shapePredictor(s.dlib_image, fl);
        s.face_encoding = faceRecognitionModelV1.compute_face_descriptor(s.dlib_image, s.face_landmark,
                                                                         settings->get_as<double>("compare_num_jitters").value_or(1));

        int i = 0;
        for (auto &enc : user.user_encodings) {
            double certainty = euclidean_distance(enc.encodings[0].data - s.face_encoding);
            if (first || certainty < best_certainty) {
                best_certainty = certainty;
                first = false;
                best_certainty_index = i;
            }

            if (certainty_threshold > certainty) {
                if (settings->get_as<double>("confirmation").value_or(true)) {
                    std::cout << '\r' << "Identified face as " << username << "          " << std::endl;
                    std::cout << "Wining model id:" << user.user_encodings[best_certainty_index].id << ", label:\""
                              << user.user_encodings[i].label << "\" (" << certainty << " < " << certainty_threshold
                              << ")"
                              << "          " << std::endl;
                }
                return PAM_SUCCESS;
            }
            i++;
        }
    }

    std::cout << '\r' << "User face unrecognized (" << best_certainty << ">" << certainty_threshold << ")"
              << "          " << std::endl;

    return PAM_SYSTEM_ERR;
}

void face_recognition::test() {
    
    timings timing = timings::instance();

    std::vector<record*> *records = timing.get_all_record();

    std::cout<<records->size()<<std::endl;

    for(record *r:*records){
        std::cout<<r->get_name()<<std::endl;
    }

    record *record = timing.get_record("Program start");
    record->finish();

    timing.print();


    cv::namedWindow("Webcam");

    cv::Mat frame;

    while (true) {
        *capture >> frame;

        cv::imshow("Webcam", frame);

        if (cv::waitKey(50) == 27) break;
    }
}

void face_recognition::camera_record() {
    snapshot snapshot;
    *capture >> snapshot;
    snapshots.push_back(snapshot);
}

double face_recognition::euclidean_distance(dlib::matrix<double> matrix) {
    double sum = 0;

    for (int i = 0; i < matrix.nr(); i++) {
        sum += pow(matrix(i, 0), 2);
    }

    return sqrt(sum);
}

/*int face_recognition::camera_record() {

        camera_sanity();

        while (continue_camera_record) {
                snapshot snapshot;
                *capture >> snapshot;
                //snapshot.image_captured = true;
                snapshots.push_back(snapshot);
        }
}*/

/*int face_recognition::snapshot_process() {

}*/

/*int face_recognition::camera_sanity() {
        if (capture->isOpened()) {
                std::cerr << "ERROR! Unable to open camera\n";
                return PAM_AUTHINFO_UNAVAIL;
        }
}*/
