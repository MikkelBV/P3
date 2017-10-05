#include "RunningSpeedCalculator.h"
#include <iostream>

using namespace cv;
using namespace std;

//i dont know if this initialization should be here, but i've left it be for now --david
AreaOfInterest AOI;

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
		cv::rectangle(
			frame,
			cv::Point(AOI.x, AOI.y),
			cv::Point(AOI.x + AOI.width, AOI.y + AOI.height),
			cv::Scalar(255, 255, 255));

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

	//handler for the event that the LEFT mouse button is pressed down
	//if its the first time that LEFT mouse button is pressed down, set AOI x and y params, and then false the firstClick bool.
	if (event == EVENT_LBUTTONDOWN) {
		if (AOI.firstClick) {
			AOI.x = x;
			AOI.y = y;
			cout << "AOI coords: " << AOI.x << ", " << AOI.y << endl;
			AOI.firstClick = false;
		}
		//otherwise set width and height with new x and y params
		else {
			AOI.width = x - AOI.x;
			AOI.height = y - AOI.y;
			cout << "AOI width and height: " << AOI.width << "px by " << AOI.height << "px" << endl;
		}
	}

	//handler for the event that RIGHT mouse button is pressed down. 
	//this resets (nullifies) the params in AOI.
	if (event == EVENT_RBUTTONDOWN) {
		cout << "Resetting AOI parameters ..." << endl;
		AOI.firstClick = true; AOI.x = NULL; AOI.y = NULL; AOI.width = NULL; AOI.height = NULL;
	}
}