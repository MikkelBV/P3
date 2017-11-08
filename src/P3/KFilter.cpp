#include "KFilter.h"
#include "TrackerInterface.h"

using namespace std;
using namespace cv;

//Colour to track
#define MIN_H_RED1 0
#define MAX_H_RED1 10
#define MIN_H_RED2 160
#define MAX_H_RED2 179


KFilter::KFilter() {
	setup();
}

KFilter::~KFilter() {
	//destroy
}

void KFilter::setup() {
	stateSize = 6; //number of states in the matrix
	measSize = 4; //coordinates to follow
	contrSize = 0;  //dimension control vector
	type = CV_32F;

	found = false;
	ticks = 0;

	kf(stateSize, measSize, contrSize, type);

	//state vector
	//x,y: centroid of object
	//v_x,v_y: speed of centroid (pixel/sec)
	//w,h: size of bounding box
	state(stateSize, 1, type); //[x,y,v_x,v_y,w,h

	//measure vector
	//z_x, z_y: centroid of identified human
	//z_w, z_h: size of identified human
	meas(measSize, 1, type);

	//Transition State Matrix A
	setIdentity(kf.transitionMatrix);

	//Measuremnt Matrix H
	kf.measurementMatrix = Mat::zeros(measSize, stateSize, type);
	kf.measurementMatrix.at(0) = 1.0f;
	kf.measurementMatrix.at(7) = 1.0f;
	kf.measurementMatrix.at(16) = 1.0f;
	kf.measurementMatrix.at(23) = 1.0f;

	//Proce Noise Covariance Matrix Q
	kf.processNoiseCov.at(0) = 1e-2;
	kf.processNoiseCov.at(7) = 1e-2;
	kf.processNoiseCov.at(14) = 2.0f;
	kf.processNoiseCov.at(21) = 1.0f;
	kf.processNoiseCov.at(28) = 1e-2;
	kf.processNoiseCov.at(35) = 1e-2;

	//Measure Noise Covariance Matrix R
	setIdentity(kf.measurementNoiseCov, Scalar(1e-1));

}

void KFilter::run(Mat _frame) {
	//Copy frame
	Mat res;
	_frame.copyTo(res);

	//Time
	double prevTick = ticks;
	ticks = (double)getTickCount();

	//seconds
	double dT = (ticks - prevTick) / getTickFrequency();

	if (found) {
		// Matrix A
		kf.transitionMatrix.at(2) = dT;
		kf.transitionMatrix.at(9) = dT;

		state = kf.predict();
		Rect predRect;
		predRect.width = state.at(4);
		predRect.height = state.at(5);
		predRect.x = state.at(0) - predRect.width / 2;
		predRect.y = state.at(1) - predRect.height / 2;

		Point2i center;
		center.x = state.at(0);
		center.y = state.at(1);
		circle(res, center, 2, CV_RGB(255, 0, 0), -1);

		rectangle(res, predRect, CV_RGB(255, 0, 0), 2);
	}

	//Noise smoothing
	Mat blur;
	GaussianBlur(_frame, blur, Size(5, 5), 3.0, 3.0);

	//HSV conversion
	Mat frmHSV;
	cvtColor(blur, frmHSV, CV_BGR2HSV);

	//Colour thresholding
	Mat lowerRange = Mat::zeros(_frame.size(), CV_8UC1);
	Mat upperRange = Mat::zeros(_frame.size(), CV_8UC1);
	inRange(frmHSV, Scalar(0, 100, 100), Scalar(10, 255, 255), lowerRange);
	inRange(frmHSV, Scalar(160, 100, 100), Scalar(179, 255, 255), upperRange);
	Mat rangeRes;
	addWeighted(lowerRange, 1.0, upperRange, 1.0, 0.0, rangeRes);

	//Improve result
	erode(rangeRes, rangeRes, Mat(), Point2i(-1, -1), 2);
	dilate(rangeRes, rangeRes, Mat(), Point2i(-1, -1), 2);

	//Contours detection
	vector<vector<Point2i>> contours;
	findContours(rangeRes, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//Filtering
	vector<vector<Point2i>> balls;
	vector<Rect> ballsBox;
	for (size_t i = 0; i < contours.size(); i++) {
		Rect bBox;
		bBox = boundingRect(contours[i]);
		float ratio = (float)bBox.width / (float)bBox.height;
		if (ratio > 1.0f)
			ratio = 1.0f / ratio;

		//Searching for a bBox almost square
		if (ratio > 0.75 && bBox.area() >= 400) {
			balls.push_back(contours[i]);
			ballsBox.push_back(bBox);
		}
	}

	//Result: Detection
	for (size_t i = 0; i < balls.size(); i++) {
		drawContours(res, balls, i, CV_RGB(20, 150, 20), 1);
		rectangle(res, ballsBox[i], CV_RGB(0, 255, 0), 2);
		
		Point2i center; 
		center.x = ballsBox[i].x + ballsBox[i].width / 2;
		center.y = ballsBox[i].y + ballsBox[i].height / 2;
		circle(res, center, 2, Scalar(20, 150, 20), -1);
		
		//could include text
	}

	//Kalman Update
	if (balls.size() == 0) {
		kf.statePost = state;
	}else {
		meas.at(0) = ballsBox[0].x + ballsBox[0].width / 2;
		meas.at(1) = ballsBox[0].y + ballsBox[0].height / 2;
		meas.at(2) = ballsBox[0].width;
		meas.at(3) = ballsBox[0].height;

		if (!found)  //first detection!!
		{
			//Initialisation
			kf.errorCovPre.at(0) = 1;
			kf.errorCovPre.at(7) = 1;
			kf.errorCovPre.at(14) = 1; 
			kf.errorCovPre.at(21) = 1; 
			kf.errorCovPre.at(28) = 1;
			kf.errorCovPre.at(35) = 1;

			state.at(0) = meas.at(0);
			state.at(1) = meas.at(1);
			state.at(2) = 0;
			state.at(3) = 0;
			state.at(4) = meas.at(2);
			state.at(5) = meas.at(3);

			kf.statePost = state;
			found = true;
		}
		else {
			kf.correct(meas); //Kalman correction
		}
	}
}

