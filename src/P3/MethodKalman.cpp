#include "MethodKalman.h"
#include "BackgroundSubtraction.h"
#include "KalmanTracker.h"
#include "MethodKeypoints.h"
#include "MethodBlobDetection.h"
#include "MethodBS.h"
#include <iostream>
#include <cmath>
#include <opencv2\videoio.hpp>
using namespace cv;
using namespace std;

MethodKalman::MethodKalman() {
	sequence = new ImageSequence();
}

MethodKalman::MethodKalman(string path) {
	sequence = new ImageSequence(path);
	filename = path;
}

double MethodKalman::process(int framesToSkip, bool resizeVideo) {
	int startTime = 0, startPosition = 0, stopTime = 0, stopPosition = 0;

	sequence->restart();
	Mat frame = sequence->nextFrame();

	KalmanTracker kalman = KalmanTracker();
	boxOrigin = Point2i(0, 0);
	Rect prevFrameRect;
	vector<int> diameters;
	Rect runner;
	vector<int> standingStill;
	bool runnerStopped = false;

	while (!frame.empty()) {

		// resize frame
		if (resizeVideo)
			resize(frame, frame, cv::Size(), 0.50, 0.50);

		// KalmanTracker
		runner = kalman.run(&frame);
		rectangle(frame, runner, Scalar(0, 255, 0));

		if (!isRunning && boxOrigin.x == 0) {
			boxOrigin = Point2i(runner.x, runner.y);
		}
		else if (runner.x - boxOrigin.x > 50 && !isRunning) {
			isRunning = true;
			startTime = sequence->getTimeStamp();
			startPosition = runner.x;
			cout << "Started running at: " << endl;
			cout << startTime << " ms" << endl;
			cout << startPosition << " px" << endl << endl;
		}
		else if (isRunning && runner.x == 0) {
			break;
		}


		if (isRunning && !runnerStopped && (runner.x - prevFrameRect.x == 0) && runner.x > 0) {
			stopTime = sequence->getTimeStamp();
			stopPosition = prevFrameRect.x;
			cout << "I think he stopped running at: " << endl;
			cout << stopTime << " ms" << endl;
			cout << stopPosition << " px" << endl << endl;
			runnerStopped = true;
		}

		if (runnerStopped) {
			standingStill.push_back(runner.x - prevFrameRect.x);

			if (standingStill.size() > 50) {
				double average = 0;

				for (double i = 0; i < standingStill.size(); i++) {
					average += standingStill[i];
				}

				average = average / standingStill.size();

				if (average < 2) {
					cout << "Stopped before frame exit with average movement the past 50 frames: " << average << endl;
					break;
				}
				else {
					cout << "i take it back... he didnt sop after all. Keep tracking!" << endl;
					standingStill.empty();
					stopTime = 0;
					stopPosition = 0;
				}

				runnerStopped = false;
			}
		}


		if (runner.width != 0 && runner.height != 0) {
			int avg = (runner.width + runner.height) / 2;
			diameters.push_back(avg);
		}

		prevFrameRect = runner;

		// display
		cv::imshow("P3", frame);

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(5)) {
			break;
		}
		else if (!pausePlayback) {
			frame = sequence->nextFrame(framesToSkip);
		}
	}

	if (stopTime == 0 && stopPosition == 0) {
		stopTime = sequence->getTimeStamp();
		stopPosition = prevFrameRect.x;
	}
	double avg = 0;

	for (size_t i = 0; i < diameters.size(); i++)
		avg += diameters[i];

	if (diameters.size() > 0)
		avg = avg / diameters.size();
	else
		avg = 0;

	double ratio = 12 / avg;
	double speedPX = abs((double)(stopPosition - startPosition)) / (((double)(stopTime - startTime)) / 1000);
	double speedCM = abs((double)(stopPosition - startPosition) * ratio) / (((double)(stopTime - startTime)) / 1000);

	cout << "My data: " << endl;
	cout << "start: " << startTime << " ms, " << startPosition << " px" << endl;
	cout << "stop: " << stopTime << " ms, " << stopPosition << " px" << endl;
	cout << endl;
	cout << "Time taken: " << ((double)(stopTime - startTime)) / 1000 << endl;
	cout << "Distance px: " << (double)(stopPosition - startPosition) << endl;
	cout << "Distance cm: " << ((double)(stopPosition - startPosition) * ratio) << endl << endl;

	return speedCM;
}

bool MethodKalman::freezeAndWait(int ms) {
	int key = waitKey(ms);

	if (key == 32) { // if keyboard input is space
		pausePlayback = !pausePlayback;
		return false;
	}
	else if (key == 8) { // if keyboard input is backspace
		sequence->restart();
		return false;
	}
	else if (key > 0) {// if it was any other key
		return true;
	}

	return false;
}

void MethodKalman::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void MethodKalman::convertToBGRA(Mat *img) {
	cvtColor(*img, *img, COLOR_GRAY2BGRA);
}