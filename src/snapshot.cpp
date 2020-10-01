#include "snapshot.h"

#include <dlib/image_transforms/assign_image.h>
#include <dlib/opencv/cv_image.h>

cv::VideoCapture &operator>>(cv::VideoCapture &input, snapshot &s) {
	input >> s.opencv_image;
	return input;
}

void snapshot::convert_image() {
	dlib::cv_image<dlib::bgr_pixel> CVImage(cvIplImage(opencv_image));
	assign_image(dlib_image, CVImage);
}
