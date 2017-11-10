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
	KalmanTracker kalman = KalmanTracker();
	boxOrigin = Point2i(0, 0);
	int startTime, startPosition, stopTime, stopPosition;
	Rect prevFrameRect;

	while (!frame.empty()) {

		// KalmanTracker
		Rect runner = kalman.run(&frame);
		rectangle(frame, runner, Scalar(0, 255, 0));
		cout << runner.x << ", ";
		if (!isRunning && runner.x == 0) {
			boxOrigin = Point2i(runner.x, runner.y);
		} else if (runner.x - boxOrigin.x > 50 && !isRunning){
			isRunning = true;
			startTime = sequence->getTimeStamp();
			startPosition = runner.x;
			cout << "isRunning " << startTime << endl;
		} else if (isRunning && runner.x == 0) {
			stopTime = sequence->getTimeStamp();
			stopPosition = prevFrameRect.x;

			/*cout
				<< endl 
				<< "stopped yew fucking twat" << endl
				<< startTime << " - " << stopTime << "ms" << endl
				<< startPosition << " - " << stopPosition << "px" << endl;*/

			speed = abs((double)(stopPosition - startPosition)) / (double)((stopTime - startTime) / 1000);

			return speed;
		}

		prevFrameRect = runner;

		// display
		imshow("P3", frame);

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40)) {
			break;
		} else if (!pausePlayback) {
			frame = sequence->nextFrame();
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
		finishStamp = sequence->getTimeStamp();
		Point2i finalPosition = areaOfInterest.getPoint1();
		int pixelMovement = finalPosition.x - boxOrigin.x; // Get change in x position from origin to finish
		speed = pixelMovement / ((finishStamp - originStamp) / 1000);
		cout << "stop: " << finishStamp << " ms" << endl;
		cout << "distance: " << pixelMovement << " px" << endl;
		return false;
}

// check if running, and if true set timestamp and isRunning
bool RunningSpeedCalculator::runnerDidStart(Rect runner) {
	
	return false;
}

void RunningSpeedCalculator::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void RunningSpeedCalculator::convertToBGRA(Mat *img) {
	cvtColor(*img, *img, COLOR_GRAY2BGRA);
}