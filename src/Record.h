#ifndef LINUXHELLO_RECORD_H
#define LINUXHELLO_RECORD_H


#include <string>
#include "Snapshot.h"
#include "darkness.h"

struct Record {
	std::string cam_path;
	std::vector<Snapshot> video_record;
	Darkness *darkness;
	cv::VideoCapture *camera;

	Record(const std::string &cam_path, double dark_threshold) {
		this->cam_path = cam_path;
		camera = new cv::VideoCapture(cam_path, cv::CAP_V4L2);
		darkness = new Darkness(dark_threshold);
	}

	void record(){
		Snapshot snapshot;
		(*camera)>>snapshot;
		video_record.push_back(snapshot);
	}

	void prepare() {
		for (auto &snapshot: video_record) {
			if (darkness->darkness_check(snapshot)) continue;
			snapshot.convert_image();
		}
	}
};


#endif //LINUXHELLO_RECORD_H
