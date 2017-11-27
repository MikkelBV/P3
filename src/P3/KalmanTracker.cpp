#include "KalmanTracker.h"


using namespace std;
using namespace cv;

//Colour to track
#define MIN_H_RED1 0
#define MAX_H_RED1 10
#define MIN_H_RED2 160
#define MAX_H_RED2 179


KalmanTracker::KalmanTracker() {
	setup();
}

void KalmanTracker::setup() {
	stateSize = 6; //number of states in the matrix
	measSize = 4; //coordinates to follow
	contrSize = 0;  //dimension control vector
	type = CV_32F;
	notFoundCount = 0;
	found = false;
	ticks = 0;

	kf = KalmanFilter(stateSize, measSize, contrSize, type);

	//state vector
	//x,y: centroid of object
	//v_x,v_y: speed of centroid (pixel/sec)
	//w,h: size of bounding box
	state = Mat(stateSize, 1, type); //[x,y,v_x,v_y,w,h

	//measure vector
	//z_x, z_y: centroid of identified human
	//z_w, z_h: size of identified human
	meas = Mat(measSize, 1, type);

	//Transition State Matrix A
	setIdentity(kf.transitionMatrix);

	//Measuremnt Matrix H
	kf.measurementMatrix = Mat::zeros(measSize, stateSize, type);
	kf.measurementMatrix.at<float>(0) = 1.0f;
	kf.measurementMatrix.at<float>(7) = 1.0f;
	kf.measurementMatrix.at<float>(16) = 1.0f;
	kf.measurementMatrix.at<float>(23) = 1.0f;

	//Proce Noise Covariance Matrix Q
	kf.processNoiseCov.at<float>(0) = 1e-2;
	kf.processNoiseCov.at<float>(7) = 1e-2;
	kf.processNoiseCov.at<float>(14) = 2.0f;
	kf.processNoiseCov.at<float>(21) = 1.0f;
	kf.processNoiseCov.at<float>(28) = 1e-2;
	kf.processNoiseCov.at<float>(35) = 1e-2;

	//Measure Noise Covariance Matrix R
	setIdentity(kf.measurementNoiseCov, Scalar(1e-1));

}

Rect KalmanTracker::run(Mat *_frame) {
	//Copy frame
	Mat res;
	_frame->copyTo(res);

	//Time
	double prevTick = ticks;
	ticks = (double)getTickCount();

	//seconds
	double dT = (ticks - prevTick) / getTickFrequency();

	Rect predRect;
	if (found) {
		// Matrix A
		kf.transitionMatrix.at<float>(2) = dT;
		kf.transitionMatrix.at<float>(9) = dT;

		state = kf.predict();
		predRect.width = state.at<float>(4);
		predRect.height = state.at<float>(5);
		predRect.x = state.at<float>(0) - predRect.width / 2;
		predRect.y = state.at<float>(1) - predRect.height / 2;

		cv::Point center;
		center.x = state.at<float>(0);
		center.y = state.at<float>(1);
		circle(res, center, 2, CV_RGB(0, 0, 255), -1);
		rectangle(res, predRect, CV_RGB(0, 0, 255), 2);
	}

	//Good features
	Mat img;
	_frame->copyTo(img);
	cvtColor(img, img, COLOR_BGR2GRAY);
	vector<Point> corners;
	goodFeaturesToTrack(img, corners, 1, 0.01, 5);

	//Bounding box
	vector<vector<Point>> features;
	features.push_back(corners);

	Rect bBox;
	bBox = boundingRect(features[0]);

	float ratio = (float)bBox.width / (float)bBox.height;
	if (ratio > 1.0f)
		ratio = 1.0f / ratio;

	//Result Detection
	rectangle(res, bBox, CV_RGB(0, 255, 0), 2);
	Point center;
	center.x = bBox.x + bBox.width / 2;
	center.y = bBox.y + bBox.height / 2;
	circle(res, center, 2, Scalar(20, 150, 20), -1);
	

	//Kalman Update
	if (features.size() == 0) {
		notFoundCount++;
		if (notFoundCount >= 10)
		{
			found = false;
		}
		else
			kf.statePost = state;
	} else {

		notFoundCount = 0;

		meas.at<float>(0) = bBox.x + bBox.width / 2;
		meas.at<float>(1) = bBox.y + bBox.height / 2;
		meas.at<float>(2) = bBox.width;
		meas.at<float>(3) = bBox.height;

		if (!found)  //first detection!!
		{
			//Initialisation
			kf.errorCovPre.at<float>(0) = 1;
			kf.errorCovPre.at<float>(7) = 1;
			kf.errorCovPre.at<float>(14) = 1;
			kf.errorCovPre.at<float>(21) = 1;
			kf.errorCovPre.at<float>(28) = 1;
			kf.errorCovPre.at<float>(35) = 1;

			state.at<float>(0) = meas.at<float>(0);
			state.at<float>(1) = meas.at<float>(1);
			state.at<float>(2) = 0;
			state.at<float>(3) = 0;
			state.at<float>(4) = meas.at<float>(2);
			state.at<float>(5) = meas.at<float>(3);

			kf.statePost = state;
			found = true;
		}
		else {
			kf.correct(meas); //Kalman correction
		}
	}
	*_frame = res;
	return predRect;
}
