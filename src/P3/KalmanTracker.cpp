#include "KalmanTracker.h"


using namespace std;
using namespace cv;

//  Colour to track
#define MIN_H_RED1 0
#define MAX_H_RED1 10
#define MIN_H_RED2 160
#define MAX_H_RED2 179

//  Constructor
KalmanTracker::KalmanTracker() {
	setup();
}

void KalmanTracker::setup() {
	stateSize = 6; 
	measSize = 4; 
	contrSize = 0; 
	type = CV_32F;
	notFoundCount = 0;
	// Used to indicate ball's previous detection, so as to activate tracking
	found = false;
	ticks = 0;

	//  Initialise Kalman filter
	kf = KalmanFilter(stateSize, measSize, contrSize, type);

	//  State vector
	//  x,y: centroid of object
	//  v_x,v_y: speed of centroid (pixel/sec)
	//  w,h: size of bounding box
	state = Mat(stateSize, 1, type); 

	//  Measure vector
	//  z_x, z_y: centroid of identified human
	//  z_w, z_h: size of identified human
	meas = Mat(measSize, 1, type);

	//  State Transition Matrix A
	//  Multiplied with the state vector creates a state vector at 
	//  a later time t
	//  Currently an identity matrix
	//  Will lateron in program contain different times (dT)
	setIdentity(kf.transitionMatrix);

	//  Measuremnt Matrix H
	//  To model sensors, that inform indirectly about the state
	//  Reads position of center of ball
	kf.measurementMatrix = Mat::zeros(measSize, stateSize, type);
	kf.measurementMatrix.at<float>(0) = 1.0f;
	kf.measurementMatrix.at<float>(7) = 1.0f;
	kf.measurementMatrix.at<float>(16) = 1.0f;
	kf.measurementMatrix.at<float>(23) = 1.0f;

	//  Process Noise Covariance Matrix Q
	//  Captures correlation. Each element of the matrix is the degree
	//  of correlation between the ith state variable and the jth state
	//  variable
	//  Q captures noise - external forces that are not known to us
	kf.processNoiseCov.at<float>(0) = 1e-2;
	kf.processNoiseCov.at<float>(7) = 1e-2;
	kf.processNoiseCov.at<float>(14) = 2.0f;
	kf.processNoiseCov.at<float>(21) = 1.0f;
	kf.processNoiseCov.at<float>(28) = 1e-2;
	kf.processNoiseCov.at<float>(35) = 1e-2;

	//  Measure Noise Covariance Matrix R 
	//  To deal with sensor noise. Due to unreliability a range of sensor
	//  readings are possible. Some states are more likely than others, 
	//  hence a covariance of this uncertainty
	setIdentity(kf.measurementNoiseCov, Scalar(1e-1));
}

Rect KalmanTracker::run(Mat *_frame) {
	//  Copy frame
	Mat res;
	_frame->copyTo(res);

	//  Time 
	double prevTick = ticks;
	ticks = (double)getTickCount();

	//  Delta time in seconds
	double deltaTime = (ticks - prevTick) / getTickFrequency();

	//  Rect object for storing the predicted ball
	Rect predRect;

	if (found) {
		//  Matrix A - used for storing the linear model
		kf.transitionMatrix.at<float>(2) = deltaTime;
		kf.transitionMatrix.at<float>(9) = deltaTime;

		//  Get the predicted rect from the KalmanFilter object
		state = kf.predict();

		//  Set the dimensions of the rect. This rect is the return value of the method
		predRect.width = state.at<float>(4);
		predRect.height = state.at<float>(5);
		predRect.x = state.at<float>(0) - predRect.width / 2;
		predRect.y = state.at<float>(1) - predRect.height / 2;

		//  Draw points to frame
		cv::Point center;
		center.x = state.at<float>(0);
		center.y = state.at<float>(1);
		circle(res, center, 2, CV_RGB(0, 0, 255), -1);
		rectangle(res, predRect, CV_RGB(0, 0, 255), 2);
	}

	//  Noise smoothing
	Mat blur;
	GaussianBlur(*_frame, blur, Size(5, 5), 3.0, 3.0);

	//  HSV conversion
	Mat frmHSV;
	cvtColor(blur, frmHSV, CV_BGR2HSV);

	//  Colour thresholding - using defined colour ranges
	Mat lowerRange = Mat::zeros(_frame->size(), CV_8UC1);
	Mat upperRange = Mat::zeros(_frame->size(), CV_8UC1);
	inRange(frmHSV, Scalar(0, 100, 100), Scalar(10, 255, 255), lowerRange);
	inRange(frmHSV, Scalar(160, 100, 100), Scalar(179, 255, 255), upperRange);
	Mat rangeRes;
	addWeighted(lowerRange, 1.0, upperRange, 1.0, 0.0, rangeRes);

	//  Morphological operations to improve result
	erode(rangeRes, rangeRes, Mat(), Point2f(-1, -1), 2);
	dilate(rangeRes, rangeRes, Mat(), Point2f(-1, -1), 2);

	imshow("Threshold", rangeRes);

	//  Contours detection of the ball
	vector<vector<cv::Point>> contours;
	findContours(rangeRes, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//  Filtering 
	vector<vector<cv::Point>> balls;
	vector<Rect> ballsBox;
	for (size_t i = 0; i < contours.size(); i++) {
		Rect bBox;
		bBox = boundingRect(contours[i]);
		float ratio = (float)bBox.width / (float)bBox.height;
		if (ratio > 1.0f)
			ratio = 1.0f / ratio;

		//  Searching for a bBox almost square
		if (ratio > 0.75 && bBox.area() >= 50) {
			balls.push_back(contours[i]);
			ballsBox.push_back(bBox);
		}
	}

	//  Result: Detection
	for (size_t i = 0; i < balls.size(); i++) {
		drawContours(res, balls, i, CV_RGB(20, 150, 20), 1);
		rectangle(res, ballsBox[i], CV_RGB(0, 255, 0), 2);

		cv::Point center; 
		center.x = ballsBox[i].x + ballsBox[i].width / 2;
		center.y = ballsBox[i].y + ballsBox[i].height / 2;
		circle(res, center, 2, Scalar(20, 150, 20), -1);
	}

	//  Kalman Update
	if (balls.size() == 0) {
		notFoundCount++;
		//  If ball is not found in more than 10 frames, tracking is stopped 
		//  and reinitialised if the ball is detected again
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

		if (!found)  //  First detection!! - Tracking is initialised
		{
			//  Initialisation
			//  Matrix P - a measure of the estimated accuracy of the state estimate
			kf.errorCovPre.at<float>(0) = 1;
			kf.errorCovPre.at<float>(7) = 1;
			kf.errorCovPre.at<float>(14) = 1;
			kf.errorCovPre.at<float>(21) = 1;
			kf.errorCovPre.at<float>(28) = 1;
			kf.errorCovPre.at<float>(35) = 1;

			//  State vector is updated using measure vector values
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
			//  Kalman correction
			kf.correct(meas); 
		}
	}
	// All changes were done on res - so as to be displayed in video
	// pointer needs to be reasigned
	*_frame = res;
	return predRect;
}

