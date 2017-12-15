#include "Speeder.h"
#include "BackgroundSubtraction.h"
#include "KalmanTracker.h"
#include "MethodKalman.h"
#include "MethodKeypoints.h"
#include "MethodBlobDetection.h"
#include "MethodBS.h"
#include <iostream>
#include <cmath>
#include <opencv2\videoio.hpp>
using namespace cv;
using namespace std;

Speeder::Speeder() {
	sequence = new ImageSequence();
}

Speeder::Speeder(string path) {
	sequence = new ImageSequence(path);
	filename = path;
}

double Speeder::process(int method, int framesToSkip, bool resizeVideo) {
	switch (method) {
		case METHOD_KALMAN:
			return methodKalman(framesToSkip, resizeVideo);
		case METHOD_BACKGROUNDSUBTRACTION:
			return methodBackgroundSubtraction(framesToSkip, resizeVideo);
		case METHOD_BLOBDETECTION:
			return methodBlobDetection(framesToSkip, resizeVideo);
		case METHOD_KEYPOINTS:
			return methodKeypoints(framesToSkip, resizeVideo);
		default:
			return 0;
	}
}

/*
 * ----------------------------------------------
 * TRACKING APPROACH METHODS
 * ----------------------------------------------
 */

double Speeder::methodKalman(int framesToSkip, bool resizeVideo) {
	MethodKalman tracker(filename);
	return tracker.process(framesToSkip, resizeVideo);
}

double Speeder::methodBackgroundSubtraction(int framesToSkip, bool resizeVideo) {
	MethodBS tracker(filename);
	return tracker.process();
}

double Speeder::methodBlobDetection(int framesToSkip, bool resizeVideo) {
	MethodBlobDetection tracker(filename);
	return tracker.process();
}

double Speeder::methodKeypoints(int framesToSkip, bool resizeVideo) {
	MethodKeypoints tracker(filename);
	return tracker.process();
}