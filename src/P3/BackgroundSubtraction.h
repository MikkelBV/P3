#pragma once

#include <opencv2/opencv.hpp>
#include "AreaOfInterest.h"

class BackgroundSubtraction {
public:
	// Constructor
	BackgroundSubtraction();
	// Tracks the runner between frames using AreaOfInterest
	void track(cv::Mat * thisFrame, cv::Mat * prevFrame);
private:
	cv::Mat frame; //current frame
	//fg masks generated with MOG and MOG2
	//https://docs.opencv.org/2.4/modules/video/doc/motion_analysis_and_object_tracking.html?highlight=backgroundsubtractorMOG#backgroundsubtractormog
	cv::Mat fgMaskMOG2;
	//MOG2 Background subtractor
	cv::Ptr<cv::BackgroundSubtractor> pMOG2; 
};

