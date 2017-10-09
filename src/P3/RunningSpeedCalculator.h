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
	std::vector<cv::Point2f> findKeyPoints(cv::Mat img);
	void drawKeyPoints(cv::Mat img, std::vector<cv::Point2f> keypoints);
	void drawAreaOfInterest(cv::Mat img, AreaOfInterest area);
	void onMouse(int x, int y, int event);

private:
	ImageSequence *sequence;
	AreaOfInterest human;
	bool pausePlayback = true;
};

