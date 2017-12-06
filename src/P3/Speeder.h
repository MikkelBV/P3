#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanTracker.h"
#include <vector>

class Speeder{

public:
	Speeder();
	Speeder(std::string path);
	double process(int method, int framesToSkip, bool resizeVideo);

	double methodKalman(int framesToSkip, bool resizeVideo);
	double methodBackgroundSubtraction(int framesToSkip, bool resizeVideo);
	double methodBlobDetection(int framesToSkip, bool resizeVideo);
	double methodFeatureMatching(int framesToSkip, bool resizeVideo);
	double methodKalmanFeatures(int framesToSkip, bool resizeVideo);
	double methodKeypoints(int framesToSkip, bool resizeVideo);
	double methodSkinDetection(int framesToSkip, bool resizeVideo);

private:
	ImageSequence *sequence;
	AreaOfInterest areaOfInterest;
	bool pausePlayback = false;
	const cv::Scalar BLUE = cv::Scalar (255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar (0, 255, 0);
	const cv::Scalar RED = cv::Scalar (0, 0, 255);
	const int RUNNING_MIN_THRESHHOLD = 8;
	int originStamp = 0; // Timestamp when runner starts
	int finishStamp = 0; // Timestamp when runner finishes
	bool isRunning = false; // Boolean that checks movement(with threshhold)
	cv::Point2i boxOrigin; // add to uml
	double speed = 0; // add to uml
	string filename = "";
};

const int METHOD_KALMAN = 0;
const int METHOD_BACKGROUNDSUBTRACTION = 1;
const int METHOD_BLOBDETECTION = 2;
const int METHOD_KEYPOINTS = 3;