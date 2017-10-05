#include "RunningSpeedCalculator.h"
#include <iostream>

using namespace cv;
using namespace std;

//i dont know if this initialization should be here, but i've left it be for now --david


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
		drawAreaOfInterest(frame, human);

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
	//if its the first time that LEFT mouse button is pressed down, set human x and y params, and then false the firstClick bool.
	if (event == EVENT_LBUTTONDOWN) {
		if (human.firstClick) {
			human.x = x;
			human.y = y;
			cout << "hooman coords: " << human.x << ", " << human.y << endl;
			human.firstClick = false;
		}
		//otherwise set width and height with new x and y params
		else {
			human.width = x - human.x;
			human.height = y - human.y;
			cout << "hooman width and height: " << human.width << "px by " << human.height << "px" << endl;
		}
	}

	//handler for the event that RIGHT mouse button is pressed down. 
	//this resets (nullifies) the params in human.
	if (event == EVENT_RBUTTONDOWN) {
		cout << "Resetting hooman parameters ..." << endl;
		human.firstClick = true; human.x = NULL; human.y = NULL; human.width = NULL; human.height = NULL;
	}
}

void RunningSpeedCalculator::drawAreaOfInterest(Mat img, AreaOfInterest area) {
	rectangle(img, Point(area.x, area.y), Point(area.x + area.width, area.y + area.height), Scalar(255, 255, 255));
}