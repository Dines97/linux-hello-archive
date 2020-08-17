#ifndef LINUXHELLO_FACE_RECOGNITION_H
#define LINUXHELLO_FACE_RECOGNITION_H


#include <chrono>
#include <thread>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv/cv_image.h>

#include <nlohmann/json.hpp>

#include <security/pam_modules.h>

#include "face_recognition_model_v1.h"
#include "snapshot.h"
#include "darkness.h"

class face_recognition {

	cv::VideoCapture *capture;
	Darkness *darkness;

	nlohmann::json j_encodings, settings;

	std::vector<snapshot> snapshots;

	dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
	dlib::shape_predictor shapePredictor;
	face_recognition_model_v1 faceRecognitionModelV1;

	bool continue_camera_record{};

	static double euclidean_distance(dlib::matrix<double> matrix);

public:
	explicit face_recognition(const nlohmann::json &settings);

	int add(const std::string &username);

	int compare(const std::string &username);

	void camera_record();

	void test();
};


#endif //LINUXHELLO_FACE_RECOGNITION_H
