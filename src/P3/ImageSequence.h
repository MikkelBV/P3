#pragma once
#include <opencv2/opencv.hpp>
#include "AreaOfInterest.h"

class ImageSequence {
public:
	ImageSequence();
	ImageSequence(std::string path);
	~ImageSequence();
	cv::Mat nextFrame();
	cv::Mat getSubImage(cv::Mat original, AreaOfInterest area);
	void restart();

private:
	cv::VideoCapture video; 
	cv::Mat currentFrame; // set this variable in nextFrame()
};

