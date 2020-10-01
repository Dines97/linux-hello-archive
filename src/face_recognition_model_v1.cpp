#include "face_recognition_model_v1.h"

face_recognition_model_v1::face_recognition_model_v1(const std::string &model_filename) {
	dlib::deserialize(model_filename) >> net;
}

dlib::matrix<double, 0, 1> face_recognition_model_v1::compute_face_descriptor(const dlib::matrix<dlib::rgb_pixel> &img,
																			  const dlib::full_object_detection &face,
																			  const int num_jitters, float padding) {
	std::vector<dlib::full_object_detection> faces(1, face);
	return compute_face_descriptors(img, faces, num_jitters, padding)[0];
}

std::vector<dlib::matrix<double, 0, 1>> face_recognition_model_v1::compute_face_descriptors(
	const dlib::matrix<dlib::rgb_pixel> &img, const std::vector<dlib::full_object_detection> &faces,
	const int num_jitters, float padding) {
	std::vector<dlib::matrix<dlib::rgb_pixel>> batch_img(1, img);
	std::vector<std::vector<dlib::full_object_detection>> batch_faces(1, faces);
	return batch_compute_face_descriptors(batch_img, batch_faces, num_jitters, padding)[0];
}

std::vector<std::vector<dlib::matrix<double, 0, 1>>> face_recognition_model_v1::batch_compute_face_descriptors(
	std::vector<dlib::matrix<dlib::rgb_pixel>> batch_imgs,
	const std::vector<std::vector<dlib::full_object_detection>> &batch_faces, const int num_jitters, float padding) {
	if (batch_imgs.size() != batch_faces.size())
		throw dlib::error("The array of images and the array of array of locations must be of the same size");

	int total_chips = 0;
	for (const auto &faces : batch_faces) {
		total_chips += faces.size();
		for (const auto &f : faces) {
			if (f.num_parts() != 68 && f.num_parts() != 5)
				throw dlib::error(
					"The full_object_detection must use the iBUG 300W 68 point face landmark style or dlib's 5 point "
					"style.");
		}
	}

	dlib::array<dlib::matrix<dlib::rgb_pixel>> face_chips;
	for (int i = 0; i < batch_imgs.size(); ++i) {
		auto &faces = batch_faces[i];
		auto &img = batch_imgs[i];

		std::vector<dlib::chip_details> dets;
		for (const auto &f : faces) dets.push_back(get_face_chip_details(f, 150, padding));
		dlib::array<dlib::matrix<dlib::rgb_pixel>> this_img_face_chips;
		extract_image_chips(img, dets, this_img_face_chips);

		for (auto &chip : this_img_face_chips) face_chips.push_back(chip);
	}

	std::vector<std::vector<dlib::matrix<double, 0, 1>>> face_descriptors(batch_imgs.size());
	if (num_jitters <= 1) {
		// extract descriptors and convert from float vectors to double vectors
		auto descriptors = net(face_chips, 16);
		auto next = std::begin(descriptors);
		for (int i = 0; i < batch_faces.size(); ++i) {
			for (int j = 0; j < batch_faces[i].size(); ++j) {
				face_descriptors[i].push_back(matrix_cast<double>(*next++));
			}
		}
	} else {
		// extract descriptors and convert from float vectors to double vectors
		auto fimg = std::begin(face_chips);
		for (int i = 0; i < batch_faces.size(); ++i) {
			for (int j = 0; j < batch_faces[i].size(); ++j) {
				auto &r = mean(mat(net(jitter_image(*fimg++, num_jitters), 16)));
				face_descriptors[i].push_back(matrix_cast<double>(r));
			}
		}
	}

	return face_descriptors;
}

std::vector<dlib::matrix<dlib::rgb_pixel>> face_recognition_model_v1::jitter_image(
	const dlib::matrix<dlib::rgb_pixel> &img, const int num_jitters) {
	std::vector<dlib::matrix<dlib::rgb_pixel>> crops;
	for (int i = 0; i < num_jitters; ++i) crops.push_back(dlib::jitter_image(img, rnd));
	return crops;
}
