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
	vector<int> diameters;

	while (!frame.empty()) {

		// KalmanTracker
		Rect runner = kalman.run(&frame);
		rectangle(frame, runner, Scalar(0, 255, 0));

		if (!isRunning && boxOrigin.x == 0) {
			boxOrigin = Point2i(runner.x, runner.y);
		} else if (runner.x - boxOrigin.x > 50 && !isRunning){
			isRunning = true;
			startTime = sequence->getTimeStamp();
			startPosition = runner.x;
		} else if (isRunning && runner.x == 0) {
			stopTime = sequence->getTimeStamp();
			stopPosition = prevFrameRect.x;

			double avg = 0;

			for (size_t i = 0; i < diameters.size(); i++) 
				avg += diameters[i];
			
			if (diameters.size() > 0)
				avg = avg / diameters.size();
			else
				avg = 0;

			double ratio = 12 / avg;
			double speedPX = abs((double)(stopPosition - startPosition)) / (double)((stopTime - startTime) / 1000);
			double speedCM = abs((double)(stopPosition - startPosition) * ratio) / (double)((stopTime - startTime) / 1000);

			return speedCM;
		}

		if (runner.width != 0 && runner.height != 0) {
			int avg = (runner.width + runner.height) / 2;
			diameters.push_back(avg);
		}

		prevFrameRect = runner;

		// display
		cv::imshow("P3", frame);

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