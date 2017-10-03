#pragma once
#include <opencv2/opencv.hpp>

// use the 

class ImageSequence {
public:
	ImageSequence();
	ImageSequence(std::string path);
	~ImageSequence();

	cv::Mat nextFrame();
private:
	cv::VideoCapture video; 
	cv::Mat currentFrame; // set this variable in nextFrame()
};

