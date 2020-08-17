#ifndef LINUXHELLO_DARKNESS_H
#define LINUXHELLO_DARKNESS_H


#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/imgproc.hpp>
#include "snapshot.h"

const static cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2, cv::Size(8, 8));

class Darkness {

	int black_tries = 0, valid_frames = 0, dark_tries = 0;
	double dark_running_total = 0, dark_threshold;

public:

	explicit Darkness(double dark_threshold);

	bool darkness_check(const snapshot &s);

	[[nodiscard]] int getBlackTries() const;

	[[nodiscard]] int getValidFrames() const;

	[[nodiscard]] int getDarkTries() const;

	[[nodiscard]] double getDarkRunningTotal() const;

	[[nodiscard]] double getDarkThreshold() const;
};


#endif //LINUXHELLO_DARKNESS_H
