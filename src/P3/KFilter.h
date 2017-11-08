#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "AreaOfInterest.h"

class KFilter {
public:
	KFilter();
	~KFilter();
	void setup();
	void run(cv::Mat _frame);
	void preprocess(cv::Mat _frame);

private:
	KalmanFilter kf;
	Mat state;
	Mat meas;

	int stateSize;
	int measSize;
	int contrSize;

	bool found;

	double prevTick;
	double ticks;
	unsigned int type;
};
