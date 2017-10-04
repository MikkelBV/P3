#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "EventHandler.h"
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
	void setAreaOfInterest(int x, int y, int width, int height);
	void setAreaOfInterest(AreaOfInterest area);
	void onMouse(int x, int y, int event);

private:
	ImageSequence *sequence;
	AreaOfInterest human;
	EventHandler eventHandler;
};

