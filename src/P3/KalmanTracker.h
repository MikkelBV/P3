#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;

class KalmanTracker {
public:
	KalmanTracker();
	void run(cv::Mat *_frame, vector<cv::Point> *features);
	vector<cv::Point> detectFeatures(cv::Mat *subImage, cv::Point2i offset);
	
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
