#include "darkness.h"

#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/imgproc.hpp>

Darkness::Darkness(double dark_threshold) { this->dark_threshold = dark_threshold; }

bool Darkness::darkness_check(const snapshot &s) {
	cv::Mat gs_frame, clahe_frame, hist;

	cv::cvtColor(s.opencv_image, gs_frame, cv::COLOR_BGR2GRAY);

	clahe->apply(gs_frame, clahe_frame);

	int hist_size = 8;
	float range[] = {0, 256};
	const float *hist_range = {range};
	bool uniform = true, accumulate = false;

	cv::calcHist(&clahe_frame, 1, nullptr, cv::Mat(), hist, 1, &hist_size, &hist_range, uniform, accumulate);

	double hist_total = cv::sum(hist)[0];
	double darkness = hist.at<float>(0) / hist_total * 100;

	if (hist_total == 0 || darkness == 100) {
		black_tries++;
		return false;
	}

	dark_running_total += darkness;
	valid_frames++;

	if (darkness > dark_threshold) {
		dark_tries++;
		return false;
	}

	return true;
}

int Darkness::getBlackTries() const { return black_tries; }

int Darkness::getValidFrames() const { return valid_frames; }

int Darkness::getDarkTries() const { return dark_tries; }

double Darkness::getDarkRunningTotal() const { return dark_running_total; }

double Darkness::getDarkThreshold() const { return dark_threshold; }
