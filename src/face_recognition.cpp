#include "face_recognition.h"

face_recognition::face_recognition(const nlohmann::json &settings) : faceRecognitionModelV1(
		"/etc/linux-hello/data/dlib_face_recognition_resnet_model_v1.dat") {

	this->settings = settings;

	darkness = new Darkness(settings["dark_threshold"]);

	dlib::deserialize("/etc/linux-hello/data/shape_predictor_5_face_landmarks.dat") >> shapePredictor;

	if (settings["auto_find_camera"]) {
		capture = new cv::VideoCapture(cv::CAP_V4L);
	} else {
		capture = new cv::VideoCapture(settings["camera_index"].get<int>(), cv::CAP_V4L);
	}
}

int face_recognition::add(const std::string &username) {

	std::string model_name = "/etc/linux-hello/models/" + username + ".dat";
	std::fstream f_models;
	f_models.open(model_name, std::ios::in);

	if (f_models.good()) {
		f_models >> j_encodings;
	}

	std::string label;
	std::cout << "Enter a label for new model: ";
	std::cin >> label;

	int valid_encodings = 0;
	double timeout = settings["add_timeout"];
	bool face_recognized = false;
	dlib::matrix<double> total_face_encoding;

	std::streamsize pp = std::cout.precision();
	std::cout << std::setprecision(1) << "Please look straight into the camera for " << timeout << " seconds"
			  << std::setprecision(pp) << std::endl;
	sleep((unsigned int) 2);

	const auto time = std::chrono::system_clock::now();
	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time).count() <
		   timeout * 1000) {
		camera_record();
	}

	capture->release();
	std::cout << "Analyzing..." << std::endl;

	for (auto &s: snapshots) {

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
																		 settings["add_num_jitters"].get<int>());

		total_face_encoding += s.face_encoding;
		valid_encodings++;

	}

	if (!face_recognized) {
		if (darkness->getValidFrames() == 0) {
			std::cout << "Camera saw only black frames - is IR emitter working?" << std::endl;
		} else if (darkness->getValidFrames() == darkness->getDarkTries()) {
			std::cout << "All frames were too dark, please check dark_threshold in config" << std::endl <<
					  "Average darkness: " << darkness->getDarkRunningTotal() / darkness->getValidFrames()
					  << ", Threshold: " << darkness->getDarkThreshold();
		} else {
			std::cout << "No face detected, aborting" << std::endl;
		}
		return 1;
	}

	std::cout << "Saving..." << std::endl;

	total_face_encoding /= valid_encodings;

	nlohmann::json j_encoding, data;

	for (int i = 0; i < total_face_encoding.nr(); i++) {
		data.push_back(total_face_encoding(i, 0));
	}

	j_encoding["time"] = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
	j_encoding["label"] = label;
	j_encoding["id"] = j_encodings.size();
	j_encoding["data"] = data;
	j_encodings.push_back(j_encoding);

	f_models.close();
	f_models.clear();
	f_models.open(model_name, std::ios::out | std::ios::trunc);

	f_models << std::setw(4) << j_encodings << std::endl;
	f_models.flush();
	f_models.close();

	std::cout << "Scan complete. Added a new model to " << username << std::endl;

	return 0;

}

int face_recognition::compare(const std::string &username) {

	std::string model_name = "/etc/linux-hello/models/" + username + ".dat";
	std::ifstream input(model_name);

	if (input.good()) {
		input >> j_encodings;
	}

	if (input.bad() || j_encodings.empty()) {
		std::cout << "No face model known" << std::endl << "Please add face model with:" << std::endl
				  << "\tsudo linux-hello --add" << std::endl;
		return PAM_USER_UNKNOWN;
	}

	if (settings["detection_notice"]) {
		std::cout << "Attempting face detection" << std::flush;
	}

	std::vector<dlib::matrix<double, 128, 1>> encodings;
	for (auto &j_encoding : j_encodings) {
		dlib::matrix<double, 128, 1> new_encoding;
		for (int j = 0; j < j_encoding["data"].size(); j++) {
			new_encoding(j, 0) = j_encoding["data"][j].get<double>();
		}
		encodings.push_back(new_encoding);
	}

	double certainty_threshold = settings["certainty_threshold"];
	double timeout = settings["timeout"];

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
		if (settings["detection_notice"])
			std::cout << '\r' << "Exceeded the time limit for face recognition         " << std::endl;

		return PAM_AUTH_ERR;
	}

	double best_certainty;
	bool first = true;

	for (auto &fl: s.face_locations) {

		s.face_landmark = shapePredictor(s.dlib_image, fl);
		s.face_encoding = faceRecognitionModelV1.compute_face_descriptor(s.dlib_image, s.face_landmark, settings["compare_num_jitters"].get<int>());

		int i = 0;

		for (auto &enc: encodings) {
			double certainty = euclidean_distance(enc - s.face_encoding);
			if (first || certainty < best_certainty) {
				best_certainty = certainty;
				first = false;
			}

			if (certainty_threshold > certainty) {
				if (settings["confirmation"].get<bool>()) {
					std::cout << '\r' << "Identified face as " << username << " (" << certainty << "<" << certainty_threshold << ")" << "          " << std::endl;
				}
				return PAM_SUCCESS;
			}
		}
	}

	std::cout << '\r' << "User face unrecognized (" << best_certainty << ">" << certainty_threshold << ")" << "          " << std::endl;

	return PAM_SYSTEM_ERR;
}

void face_recognition::test() {

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
