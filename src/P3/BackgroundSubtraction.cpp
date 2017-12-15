#pragma once

#include "BackgroundSubtraction.h"
#include <opencv2/opencv.hpp>

//create BackgroundSubtractor objects
BackgroundSubtraction::BackgroundSubtraction() {
	pMOG2 = cv::createBackgroundSubtractorMOG2();
}


void BackgroundSubtraction::track(cv::Mat *thisFrame, cv::Mat *prevFrame) {
	frame = *thisFrame;

	//update the background model
	pMOG2->apply(frame, fgMaskMOG2);

	// cv::imshow("Background Subtraction", fgMaskMOG2);
	*thisFrame = fgMaskMOG2;
}