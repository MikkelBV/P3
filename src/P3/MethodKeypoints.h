#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanFilterClass.h"
#include <vector>

/*	This class implements the keypoint method which uses goodFeaturesToTrack
	to detect points to track. The points are detected in an area of interest
	drawn by the user. This is done by implementing the areaOfInterest class
*/

class MethodKeypoints {

public:
	// Default onstructor
	MethodKeypoints();
	// Constructor with file path for video
	MethodKeypoints(std::string path);
	// Processes the video with keypoints approach
	double process();
	void convertToGreyscale(cv::Mat *img);
	void convertToBGRA(cv::Mat *img);
	// Pauses and restarts video
	bool freezeAndWait(int ms);
	// Stores the detected keypoints found by goodFeaturesToTrack
	std::vector<cv::Point2i> findKeyPoints(cv::Mat img);
	// Draws the detected keypoints on frame
	void drawKeyPoints(cv::Mat img, std::vector<cv::Point2i> keypoints);
	// Draws the area of interest given by the user
	void drawAreaOfInterest(cv::Mat img);
	// Detects mouse events so as to draw area of interest
	void onMouse(int x, int y, int event);
	// Compares detected keypoints to determine movement of runner
	cv::Point2i compareKeypoints(std::vector<cv::Point2i> thisFrame, std::vector<cv::Point2i> lastFrame);
	// ???
	cv::Mat getFrameForSetup(); 
	// Checks if runner is still running
	bool stillRunning(cv::Mat frame);
	// Checks if runner started running
	bool runnerDidStart(); 
	// ???
	static void mouseHandler(int event, int x, int y, int flags, void* userData) {
		MethodKeypoints *tracker = static_cast<MethodKeypoints *> (userData);
		tracker->onMouse(x, y, event);
	};

private:
	// ???
	KalmanFilterClass KF(cv::Mat _frame);
	// ???
	ImageSequence *sequence;
	// Area of interest to search keypoints in, given by user
	AreaOfInterest areaOfInterest;
	// To enable pausing of video
	bool pausePlayback = false;
	// RGB Colours
	const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
	const cv::Scalar RED = cv::Scalar(0, 0, 255);
	// ???
	const int FILTER_EQUALISATION = 0;
	// Pixel threshold to determine if runner is running
	const int RUNNING_MIN_THRESHHOLD = 8;
	// Timestamp when runner starts
	int originStamp = 0; 
	// Timestamp when runner finishes
	int finishStamp = 0; 
	// Boolean that checks movement using threshold
	bool isRunning = false; 
	// Stores top-left corner of area of interest
	cv::Point2i boxOrigin; 
	// Runner's speed
	double speed; 
};