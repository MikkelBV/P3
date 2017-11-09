#include "RunningSpeedCalculator.h"
#include "BackgroundSubtraction.h"
#include "KalmanTracker.h"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

RunningSpeedCalculator::RunningSpeedCalculator() {
	sequence = new ImageSequence();
}

RunningSpeedCalculator::RunningSpeedCalculator(string path) {
	sequence = new ImageSequence(path);
}

double RunningSpeedCalculator::process() {
	speed = 0; // what were trying to find

	sequence->restart();
	Mat frame = sequence->nextFrame();

	KalmanTracker kf = KalmanTracker(); //Kalman
	boxOrigin = areaOfInterest.getPoint1();

	while (!frame.empty()) {

		// check if runner stopped running
		if (!stillRunning(frame))
			break;

		//KalmanTracker
		kf.run(&frame);

		
		// if not already running, check if running and set time stamp if true
		if (!isRunning) {
			runnerDidStart();
		}

		// display
		cv::imshow("P3", frame);

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40)) {
			break;
		} else if (!pausePlayback) {
			frame = sequence->nextFrame(3);
		}
	}
	
	return speed;
}

bool RunningSpeedCalculator::freezeAndWait(int ms) {
	int key = waitKey(ms);
	
	if (key == 32) { // if keyboard input is space
		pausePlayback = !pausePlayback;
		return false;
	} else if (key == 8) { // if keyboard input is backspace
		sequence->restart();
		return false;
	} else if (key > 0) {// if it was any other key
		return true;
	}

	return false;
}

// check if still running, and if not get time and set speed
bool RunningSpeedCalculator::stillRunning(Mat frame) {
	if (areaOfInterest.outOfBoundsOffset(frame.cols, frame.rows)) {
		finishStamp = sequence->getTimeStamp();
		Point2i finalPosition = areaOfInterest.getPoint1();
		int pixelMovement = finalPosition.x - boxOrigin.x; // Get change in x position from origin to finish
		speed = pixelMovement / ((finishStamp - originStamp) / 1000);
		cout << "stop: " << finishStamp << " ms" << endl;
		cout << "distance: " << pixelMovement << " px" << endl;
		return false;
	}
	return true;
}

// check if running, and if true set timestamp and isRunning
bool RunningSpeedCalculator::runnerDidStart() {
	Point2i currentPosition = areaOfInterest.getPoint1();
	int xdiff = currentPosition.x - boxOrigin.x;

	if (xdiff > RUNNING_MIN_THRESHHOLD) {
		isRunning = true;
		originStamp = sequence->getTimeStamp();
		cout << "start: " << originStamp << " ms" << endl;
		return true;
	}

	return false;
}

void RunningSpeedCalculator::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void RunningSpeedCalculator::convertToBGRA(Mat *img) {
	cvtColor(*img, *img, COLOR_GRAY2BGRA);
}