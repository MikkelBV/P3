#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanTracker.h"
#include <vector>

class RunningSpeedCalculator {

public:
	RunningSpeedCalculator();
	RunningSpeedCalculator(std::string path);
	double process();
	void convertToGreyscale(cv::Mat *img);
	void convertToBGRA(cv::Mat *img);
	bool freezeAndWait(int ms);
	bool stillRunning(cv::Mat frame); // add to uml
	bool runnerDidStart(); // add to uml

private:
	ImageSequence *sequence;
	AreaOfInterest areaOfInterest;
	bool pausePlayback = false;
	const cv::Scalar BLUE = cv::Scalar (255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar (0, 255, 0);
	const cv::Scalar RED = cv::Scalar (0, 0, 255);
	const int RUNNING_MIN_THRESHHOLD = 8;
	int originStamp = 0; // Timestamp when runner starts
	int finishStamp = 0; // Timestamp when runner finishes
	bool isRunning = false; // Boolean that checks movement(with threshhold)
	cv::Point2i boxOrigin; // add to uml
	double speed; // add to uml
};

