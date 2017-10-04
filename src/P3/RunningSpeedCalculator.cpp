#include "RunningSpeedCalculator.h"
#include <iostream>

using namespace cv;
using namespace std;

RunningSpeedCalculator::RunningSpeedCalculator() {
	sequence = new ImageSequence();
	cout << "loading..." << endl;
}

RunningSpeedCalculator::RunningSpeedCalculator(string path) {
	sequence = new ImageSequence(path);
}

RunningSpeedCalculator::~RunningSpeedCalculator(){
	// destructor
}

double RunningSpeedCalculator::process() {
	Mat frame = sequence->nextFrame();

	while (!frame.empty()) {

		convertToGreyscale(&frame);
		drawKeyPoints(frame, findKeyPoints(frame));
		
		imshow("P3", frame);

		// stop playing if user presses keyboard
		if (freezeAndWait(40))
			break;
		else
			frame = sequence->nextFrame();
	}
	
	destroyAllWindows();
	return 0.0;
}

void RunningSpeedCalculator::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void RunningSpeedCalculator::convertToBGRA(Mat *img) {
	// convert from grayscale to BGRA
}

bool RunningSpeedCalculator::freezeAndWait(int ms) {
	// return true if user has pressed key
	waitKey(ms);

	return false;
}

vector<Point2f> RunningSpeedCalculator::findKeyPoints(Mat img) {
	vector< Point2f > corners;
	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 10.;

	goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance);

	return corners;
}

void RunningSpeedCalculator::drawKeyPoints(Mat img, vector<Point2f> keypoints) {
	for (size_t i = 0; i < keypoints.size(); i++) {
		cv::circle(img, keypoints[i], 6, cv::Scalar(255, 0, 0), 2);
	}
}

void RunningSpeedCalculator::onMouse(int x, int y, int event) {

}