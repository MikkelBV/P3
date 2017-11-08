#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

class KFilter {
public:
	KFilter();
	~KFilter();
	void run(cv::Mat _frame);
	
private:
	int stateSize;
	int measSize;
	int contrSize;
	int type;

	KalmanFilter kf;
	Mat state;
	Mat meas;

	bool found;

	double prevTick;
	double ticks;


	Point2i center;

	void setup();
};
