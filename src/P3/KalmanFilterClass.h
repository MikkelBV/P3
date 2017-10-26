#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "AreaOfInterest.h"

class KalmanFilterClass {
public:
	KalmanFilterClass(cv::Mat _frame);
	~KalmanFilterClass();
	void setup();
	void run(cv::Mat _frame);
	void preprocess(cv::Mat _frame);
	//cv::KalmanFilter KF(int _state, int _meas, int _contr, int _type);
	//cv::Mat state(int _state, int j, int _type); // [x, y, v_x, v_y, w, h] 
	//cv::Mat meas(int _meas, int j, int _type); // [z_x, z_y, z_w, z_h]

private:
	int stateSize;
	int measSize;
	int contrSize;
	unsigned int type = CV_32F;
};
