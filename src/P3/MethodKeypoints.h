#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanFilterClass.h"
#include <vector>

class MethodKeypoints {

public:
	MethodKeypoints();
	MethodKeypoints(std::string path);
	double process();
	void convertToGreyscale(cv::Mat *img);
	void convertToBGRA(cv::Mat *img);
	bool freezeAndWait(int ms);
	std::vector<cv::Point2i> findKeyPoints(cv::Mat img);
	void drawKeyPoints(cv::Mat img, std::vector<cv::Point2i> keypoints);
	void drawAreaOfInterest(cv::Mat img);
	void onMouse(int x, int y, int event);
	cv::Point2i compareKeypoints(std::vector<cv::Point2i> thisFrame, std::vector<cv::Point2i> lastFrame);
	cv::Mat getFrameForSetup(); // add to UML
	bool stillRunning(cv::Mat frame); // add to uml
	bool runnerDidStart(); // add to uml

	static void mouseHandler(int event, int x, int y, int flags, void* userData) {
		MethodKeypoints *tracker = static_cast<MethodKeypoints *> (userData);
		tracker->onMouse(x, y, event);
		// std::cout << "static ";
	};

private:
	KalmanFilterClass KF(cv::Mat _frame);
	ImageSequence *sequence;
	AreaOfInterest areaOfInterest;
	bool pausePlayback = false;
	const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
	const cv::Scalar RED = cv::Scalar(0, 0, 255);
	//cv::Point2i prevpoint1 = cv::Point2i(0, 0);
	//cv::Point2i prevpoint2 = cv::Point2i(1, 1);
	const int FILTER_EQUALISATION = 0;
	const int RUNNING_MIN_THRESHHOLD = 8;
	int originStamp = 0; // Timestamp when runner starts
	int finishStamp = 0; // Timestamp when runner finishes
	bool isRunning = false; // Boolean that checks movement(with threshhold)
	cv::Point2i boxOrigin; // add to uml
	double speed; // add to uml
};