#include "RunningSpeedCalculator.h"
#include <iostream>

using namespace cv;
using namespace std;

RunningSpeedCalculator::RunningSpeedCalculator() {
	sequence = new ImageSequence();
}

RunningSpeedCalculator::RunningSpeedCalculator(string path) {
	sequence = new ImageSequence(path);
}

RunningSpeedCalculator::~RunningSpeedCalculator() {
	// destructor
}

double RunningSpeedCalculator::process() {
	Mat frame = sequence->nextFrame();
	vector<Point2f> lastFramesKeypoints;

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// process image
		Mat subImage = sequence->getSubImage(frame, human);
		vector<Point2f> keypoints = findKeyPoints(subImage);

		// move areaofinterest here
			// line 21: lastFramesKeypoints stores the keypoints of.... you guessed it: last frame
			// the idea is to compare this frames keypoints (just called 'keypoints') with last frames
			// line 87 - 89: these parameters can be adjusted to change how goodFeaturesToTrack finds keypoints. Look at provided link for more info
			// use this new function to move the areaofinterest 
				// human.move(1, 1);

		// draw
		convertToBGRA(&frame);
		drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame, human);

		// display
		imshow("subImage", subImage);
		imshow("P3", frame);

		// set last frame keypoints to this frame keypoints before getting next frame
		lastFramesKeypoints = keypoints;

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
			frame = sequence->nextFrame();
	}

	destroyAllWindows();
	return 0.0;
}

void RunningSpeedCalculator::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void RunningSpeedCalculator::convertToBGRA(Mat *img) {
	cvtColor(*img, *img, COLOR_GRAY2BGRA);
}

bool RunningSpeedCalculator::freezeAndWait(int ms) {
	int key = waitKey(ms);
	
	if (key == 32) { // if keyboard input is space
		pausePlayback = !pausePlayback;
		return false;
	} else if (key == 8) { // if keyboard input is backspace
		sequence->restart();
		return false;
	} else if (key > 0) // if it was any other key
		return true;
	else // if no key was pressed
		return false;
}

vector<Point2f> RunningSpeedCalculator::findKeyPoints(Mat img) {
	// https://docs.opencv.org/2.4.13.2/modules/imgproc/doc/feature_detection.html#goodfeaturestotrack 
	vector< Point2f > corners;
	int maxCorners = 5;
	double qualityLevel = 0.01;
	double minDistance = 3.;

	goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance);

	// iterate over each keypoint and add the offset
	for (auto &corner : corners) {
		corner.x += human.getPoint1().x;
		corner.y += human.getPoint1().y;
	}

	return corners;
}

void RunningSpeedCalculator::drawKeyPoints(Mat img, vector<Point2f> keypoints) {
	for (size_t i = 0; i < keypoints.size(); i++) {
		circle(img, keypoints[i], 6, BLUE, 2);
	}
}

void RunningSpeedCalculator::onMouse(int x, int y, int event) {
	switch (event) {
	case EVENT_LBUTTONDOWN:
		human.set(x, y);
		break;
	case EVENT_RBUTTONDOWN:
		human.reset();
		break;
	default:
		break;
	}
}

void RunningSpeedCalculator::drawAreaOfInterest(Mat img, AreaOfInterest area) {
	rectangle(img, area.getPoint1(), area.getPoint2(), RED, 2);
}