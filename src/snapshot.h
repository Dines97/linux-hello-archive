//
// Created by denis on 3.08.2020.
//

#ifndef LINUXHELLO_SNAPSHOT_H
#define LINUXHELLO_SNAPSHOT_H

#include <vector>
#include <dlib/geometry/rectangle.h>
#include <opencv2/core/mat.hpp>
#include <dlib/image_processing/full_object_detection.h>
#include <opencv2/videoio.hpp>

class snapshot {

public:

	std::vector<dlib::rectangle> face_locations;
	dlib::rectangle face_location;

	cv::Mat opencv_image;

	dlib::matrix<dlib::rgb_pixel> dlib_image;
	dlib::matrix<double> face_encoding;
	dlib::full_object_detection face_landmark;

	void convert_image();

	friend cv::VideoCapture &operator>>(cv::VideoCapture &input, snapshot &s);

};


#endif //LINUXHELLO_SNAPSHOT_H
