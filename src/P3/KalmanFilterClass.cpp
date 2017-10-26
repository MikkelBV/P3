#include "KalmanFilterClass.h"
#include "TrackerInterface.h"

using namespace std;
using namespace cv;

KalmanFilterClass::KalmanFilterClass(Mat _frame) {
	//setup();
	//preprocess(_frame);
	run(_frame);
}

KalmanFilterClass::~KalmanFilterClass() {
	//destroy
}

void KalmanFilterClass::setup(){
	//maybe unused
}

void KalmanFilterClass::run(Mat _frame) {
	int stateSize = 6;
	int measSize = 4;
	int contrSize = 4;
	int type = CV_32F;
	KalmanFilter KF(stateSize, measSize, contrSize, type);

	Mat state(stateSize, 1, type); // [x, y, v_x, v_y, w, h] 
	Mat meas(measSize, 1, type); // [z_x, z_y, z_w, z_h]

	setIdentity(KF.transitionMatrix);
	KF.measurementMatrix = Mat::zeros(measSize, stateSize, type);
	KF.measurementMatrix.at<float>(0) = 1.0f;
	KF.measurementMatrix.at<float>(7) = 1.0f;
	KF.measurementMatrix.at<float>(16) = 1.0f;
	KF.measurementMatrix.at<float>(23) = 1.0f;

	KF.processNoiseCov.at<float>(0) = 1e-2;
	KF.processNoiseCov.at<float>(7) = 1e-2;
	KF.processNoiseCov.at<float>(14) = 5.0f;
	KF.processNoiseCov.at<float>(21) = 5.0f;
	KF.processNoiseCov.at<float>(28) = 1e-2;
	KF.processNoiseCov.at<float>(35) = 1e-2;
	setIdentity(KF.measurementNoiseCov, cv::Scalar(1e-1));

	double ticks = 0;
	bool found = false;
	int notFound = 0;

	double precTick = ticks;
	ticks = (double)getTickCount();
	double dT = (ticks - precTick) / getTickFrequency();

	if (found) {
		KF.transitionMatrix.at<float>(2) = dT;
		KF.transitionMatrix.at<float>(9) = dT;

		state = KF.predict();

		cv::Rect predRect;
		predRect.width = state.at<float>(4);
		predRect.height = state.at<float>(5);
		predRect.x = state.at<float>(0) - predRect.width / 2;
		predRect.y = state.at<float>(1) - predRect.height / 2;
	}

	//color thresholding
	Mat rangeRes = Mat::zeros(_frame.size(), CV_8UC1);
	inRange(_frame, Scalar(225, 225, 225), Scalar(255, 255, 255), rangeRes);

	//improving?
	//erode(rangeRes, rangeRes, Mat(), Point(-1, -1), 2);
	//dilate(rangeRes, rangeRes, Mat(), Point(-1, -1), 2);

	//contours?
	vector<vector<cv::Point> > contours;
	findContours(rangeRes, contours, CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_NONE);

	vector<vector<cv::Point> > balls;
	vector<cv::Rect> ballsBox;
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::Rect bBox;
		bBox = cv::boundingRect(contours[i]);

		float ratio = (float)bBox.width / (float)bBox.height;
		if (ratio > 1.0f)
			ratio = 1.0f / ratio;

		// Searching for a bBox almost square
		if (ratio > 0.75 && bBox.area() >= 400)
		{
			balls.push_back(contours[i]);
			ballsBox.push_back(bBox);
		}
	}

	for (size_t i = 0; i < balls.size(); i++)
	{
		cv::drawContours(_frame, balls, i, CV_RGB(20, 150, 20), 1);
		cv::rectangle(_frame, ballsBox[i], CV_RGB(0, 255, 0), 2);

		cv::Point center;
		center.x = ballsBox[i].x + ballsBox[i].width / 2;
		center.y = ballsBox[i].y + ballsBox[i].height / 2;
		cv::circle(_frame, center, 2, CV_RGB(20, 150, 20), -1);

		stringstream sstr;
		sstr << "(" << center.x << "," << center.y << ")";
		cv::putText(_frame, sstr.str(),
			cv::Point(center.x + 3, center.y - 3),
			cv::FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
	}

	if (balls.size() == 0)
	{
		//notFoundCount++;
		//cout << "notFoundCount:";// << notFoundCount << endl;
		//if (notFoundCount >= 100)
		{
			found = false;
		}
		/*else
		KF.statePost = state;*/
	}
	else
	{
		//notFoundCount = 0;

		meas.at<float>(0) = ballsBox[0].x + ballsBox[0].width / 2;
		meas.at<float>(1) = ballsBox[0].y + ballsBox[0].height / 2;
		meas.at<float>(2) = (float)ballsBox[0].width;
		meas.at<float>(3) = (float)ballsBox[0].height;

		if (!found) // First detection!
		{
			// >>>> Initialization
			KF.errorCovPre.at<float>(0) = 1; // px
			KF.errorCovPre.at<float>(7) = 1; // px
			KF.errorCovPre.at<float>(14) = 1;
			KF.errorCovPre.at<float>(21) = 1;
			KF.errorCovPre.at<float>(28) = 1; // px
			KF.errorCovPre.at<float>(35) = 1; // px

			state.at<float>(0) = meas.at<float>(0);
			state.at<float>(1) = meas.at<float>(1);
			state.at<float>(2) = 0;
			state.at<float>(3) = 0;
			state.at<float>(4) = meas.at<float>(2);
			state.at<float>(5) = meas.at<float>(3);
			// <<<< Initialization

			KF.statePost = state;

			found = true;
		}
		else
			KF.correct(meas); // Kalman Correction

		cout << "Measure matrix:" << endl << meas << endl;
	}
}

void KalmanFilterClass::preprocess(Mat _frame) {
	threshold(_frame, _frame, 50, 255, CV_THRESH_BINARY);
	medianBlur(_frame, _frame, 1);
}