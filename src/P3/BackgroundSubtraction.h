#pragma once

#include <opencv2/opencv.hpp>
#include "AreaOfInterest.h"
#include "TrackerInterface.h"

class BackgroundSubtraction {
public:
	BackgroundSubtraction();
	void track(cv::Mat * thisFrame, cv::Mat * prevFrame, AreaOfInterest aoi);
private:
	cv::Mat frame; //current frame
				   //fg masks generated with MOG and MOG2
				   //https://docs.opencv.org/2.4/modules/video/doc/motion_analysis_and_object_tracking.html?highlight=backgroundsubtractorMOG#backgroundsubtractormog
	cv::Mat fgMaskMOG2;
	cv::Ptr<cv::BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
};

