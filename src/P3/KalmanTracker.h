#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;

class KalmanTracker {
public:
	KalmanTracker();
	//  Tracks a red marker in a frame. Returns a 
	//  rectangle surrounding the marker using Kalman filter
	cv::Rect run(cv::Mat *_frame);
	
private:
	//  Number of states saved to create linear model
	int stateSize;
	//  Number of coordinates to track
	int measSize;
	//  Dimension of the control vector
	int contrSize;
	//  Matrix type
	int type;
	//  Used to initialise and call KalmanFilter functions
	cv::KalmanFilter kf;
	//  Matrix to store the states of model
	cv::Mat state;
	//  Matrix to store the coordinates of ball
	cv::Mat meas;
	//  For controlling purposes
	int notFoundCount;
	//  To state whether ball was detected in frame
	bool found;
	//  Number of ticks in previous frame
	double prevTick;
	//  Number of ticks in the current frame
	double ticks;
	//  Point that marks center of the ball
	cv::Point2i center;
	//  Setups all necessary variables for tracking. 
	//  Private as it is called in constructor
	void setup();
};
