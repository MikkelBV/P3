#pragma once
#include <opencv2/opencv.hpp>
#include "AreaOfInterest.h"

/*
This class reads the video and contains util functions
to apply on videos.
*/

class ImageSequence {
public:
	// Captures video from webcam
	ImageSequence();

	// Gets video from given path
	ImageSequence(std::string path);

	// Destructor
	~ImageSequence();

	// Gets next frame in video
	cv::Mat nextFrame();

	// Skips the number of frames given
	cv::Mat nextFrame(int framesToSkip);

	// Return the image in the area of interest given by user
	cv::Mat getSubImage(cv::Mat original, AreaOfInterest area);

	// Returns the video
	cv::VideoCapture* getVideo();

	// Restarts the video
	void restart();

	// Returns the time stamp 
	int getTimeStamp();

private:
	// Video to work with
	cv::VideoCapture video; 

	// Set this variable in nextFrame()
	cv::Mat currentFrame; 
};

