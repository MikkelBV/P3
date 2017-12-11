#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include "ImageSequence.h"
#include "AreaOfInterest.h"
#include "KalmanFilterClass.h"
#include <vector>

class MethodBlobDetection {

public:
	// Default constructor - uses webcam as video input
	MethodBlobDetection();

	// Constructor with give path for video
	MethodBlobDetection(std::string path);

	// Runs blo detection approach
	double process();

	// convert from BGRA to greyscale
	void convertToGreyscale(cv::Mat *img);

	// Pauses and restarts video
	bool freezeAndWait(int ms);

	// Stores the detected keypoints found by goodFeaturesToTrack
	std::vector<cv::Point2i> findKeyPoints(cv::Mat img);

	// Draws the detected keypoints on frame
	void drawKeyPoints(cv::Mat img, std::vector<cv::Point2i> keypoints);

	// Draws the area of interest given by the user
	void drawAreaOfInterest(cv::Mat img);

	// handles mouse events so as to draw area of interest
	void onMouse(int x, int y, int event);

	// Compares detected keypoints to determine movement of runner
	cv::Point2i compareKeypoints(std::vector<cv::Point2i> thisFrame, std::vector<cv::Point2i> lastFrame);

	// Checks if runner is still running
	bool stillRunning(cv::Mat frame); 

	// Checks if runner started running
	bool runnerDidStart(); 

	// Setups blobl detector to extract BLOBs from image
	cv::Ptr<cv::SimpleBlobDetector> setupBlobDetector();

	// detects mouse events and passes them into onMouse
	static void mouseHandler(int event, int x, int y, int flags, void* userData) {
		MethodBlobDetection *tracker = static_cast<MethodBlobDetection *> (userData);
		tracker->onMouse(x, y, event);
	};

private:
	// the video
	ImageSequence *sequence;

	// Area of interest to search keypoints in, given by user
	AreaOfInterest areaOfInterest;

	// To enable pausing of video
	bool pausePlayback = false;

	// BGR Colours
	const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
	const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
	const cv::Scalar RED = cv::Scalar(0, 0, 255);

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

	// Extracts blobs from a image
	cv::Ptr<cv::SimpleBlobDetector> blobDetector;
};