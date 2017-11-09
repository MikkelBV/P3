#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;

class KFilter {
public:
	KFilter();
	~KFilter();
	void run(cv::Mat *_frame);
	
private:
	int stateSize;
	int measSize;
	int contrSize;
	int type;
	cv::KalmanFilter kf;
	cv::Mat state;
	cv::Mat meas;
	int notFoundCount;
	bool found;
	double prevTick;
	double ticks;
	cv::Point2i center;
	void setup();
};
