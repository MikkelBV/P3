#pragma once
#include <opencv2/opencv.hpp>
#include "AreaOfInterest.h"

class TrackerInterface {
public:
	virtual cv::Point2i track(cv:: Mat *thisFrame, cv::Mat *prevFrame, AreaOfInterest aoi) = 0;
};