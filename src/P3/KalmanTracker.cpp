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

	//Noise smoothing
	Mat blur;
	GaussianBlur(*_frame, blur, Size(5, 5), 3.0, 3.0);

	//HSV conversion
	Mat frmHSV;
	cvtColor(blur, frmHSV, CV_BGR2HSV);

	//Colour thresholding
	Mat lowerRange = Mat::zeros(_frame->size(), CV_8UC1);
	Mat upperRange = Mat::zeros(_frame->size(), CV_8UC1);
	inRange(frmHSV, Scalar(0, 100, 100), Scalar(10, 255, 255), lowerRange);
	inRange(frmHSV, Scalar(160, 100, 100), Scalar(179, 255, 255), upperRange);
	Mat rangeRes;
	addWeighted(lowerRange, 1.0, upperRange, 1.0, 0.0, rangeRes);

	//Improve result
	erode(rangeRes, rangeRes, Mat(), Point2f(-1, -1), 2);
	dilate(rangeRes, rangeRes, Mat(), Point2f(-1, -1), 2);

	imshow("Threshold", rangeRes);

	//Contours detection
	vector<vector<cv::Point>> contours;
	findContours(rangeRes, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//Filtering
	vector<vector<cv::Point>> balls;
	vector<Rect> ballsBox;
	for (size_t i = 0; i < contours.size(); i++) {
		Rect bBox;
		bBox = boundingRect(contours[i]);
		float ratio = (float)bBox.width / (float)bBox.height;
		if (ratio > 1.0f)
			ratio = 1.0f / ratio;

		//Searching for a bBox almost square
		if (ratio > 0.75 && bBox.area() >= 10) {
			balls.push_back(contours[i]);
			ballsBox.push_back(bBox);
		}
	}

	//Result: Detection
	for (size_t i = 0; i < balls.size(); i++) {
		drawContours(res, balls, i, CV_RGB(20, 150, 20), 1);
		rectangle(res, ballsBox[i], CV_RGB(0, 255, 0), 2);

		cv::Point center; 
		center.x = ballsBox[i].x + ballsBox[i].width / 2;
		center.y = ballsBox[i].y + ballsBox[i].height / 2;
		circle(res, center, 2, Scalar(20, 150, 20), -1);
		//could include text
	}

	//Kalman Update
	if (balls.size() == 0) {
		notFoundCount++;
		if (notFoundCount >= 10)
		{
			found = false;
		}
		else
			kf.statePost = state;
	} else {

		notFoundCount = 0;

		meas.at<float>(0) = ballsBox[0].x + ballsBox[0].width / 2;
		meas.at<float>(1) = ballsBox[0].y + ballsBox[0].height / 2;
		meas.at<float>(2) = ballsBox[0].width;
		meas.at<float>(3) = ballsBox[0].height;

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

