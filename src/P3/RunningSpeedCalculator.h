#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include <vector>

class RunningSpeedCalculator {

public:
	RunningSpeedCalculator();
	RunningSpeedCalculator(std::string path);
	~RunningSpeedCalculator();
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
	void preprocess(int filterType);

private:
	ImageSequence *sequence;
	AreaOfInterest areaOfInterest;
	bool pausePlayback = false;
	const cv::Scalar BLUE = cv::Scalar (255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar (0, 255, 0);
	const cv::Scalar RED = cv::Scalar (0, 0, 255);
	//cv::Point2i prevpoint1 = cv::Point2i(0, 0);
	//cv::Point2i prevpoint2 = cv::Point2i(1, 1);
	int FILTER_EQUALISATION = 0;
};

