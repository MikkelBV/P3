#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanTracker.h"
#include <vector>

class Speeder{

public:
	// Default constructor
	Speeder();

	// Constructor with file path for video
	Speeder(std::string path);

	// Processes the video depending on type of method, number of frames to skip and the 
	double process(int method, int framesToSkip, bool resizeVideo);

	// Calls method that uses kalman filter and colour thresholding
	double methodKalman(int framesToSkip, bool resizeVideo);

	// Calls method that uses background subtraction
	double methodBackgroundSubtraction(int framesToSkip, bool resizeVideo);

	// Calls method that uses blob analysis
	double methodBlobDetection(int framesToSkip, bool resizeVideo);
	
	// Calls method that uses keypoint detection
	double methodKeypoints(int framesToSkip, bool resizeVideo);
	

private:
	// The video
	ImageSequence *sequence;

	// Area of interest to search keypoints in, given by user
	AreaOfInterest areaOfInterest;

	// To enable pausing of video
	bool pausePlayback = false;

	// BGR Colours
	const cv::Scalar BLUE = cv::Scalar (255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar (0, 255, 0);
	const cv::Scalar RED = cv::Scalar (0, 0, 255);

	// Pixel threshold to determine if runner is running
	const int RUNNING_MIN_THRESHHOLD = 8;

	// Timestamp when runner starts
	int originStamp = 0; 

	// Timestamp when runner finishes
	int finishStamp = 0; 

	// Compares detected keypoints to determine movement of runner
	bool isRunning = false; 

	// Stores top-left corner of area of interest
	cv::Point2i boxOrigin; 

	// Runner's speed
	double speed = 0;

	// File path to video
	string filename = "";
};

// Method constants
const int METHOD_KALMAN = 0;
const int METHOD_BACKGROUNDSUBTRACTION = 1;
const int METHOD_BLOBDETECTION = 2;
const int METHOD_KEYPOINTS = 3;