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
	vector<Point2i> lastFramesKeypoints;

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// process image
		Mat subImage = sequence->getSubImage(frame, human);
		vector<Point2i> keypoints = findKeyPoints(subImage);
		
		// compare keypoints and move areaOfInterest
		double avg = 0;
		int numComparableKeypoints = 0; // use this variable to calculate the average instead of dividing by keypointsLength which would take all elements into the calculation

			// store size locally to avoid a size() call every iteration of loop
		int keypointsLength = keypoints.size();
		int lastKeypointsLength = lastFramesKeypoints.size(); 

			// iterate over keypoints. both keypoints vectors will always be the same size.
		for (size_t i = 0; i < keypointsLength; i++) { 
			double this_x = (double) keypoints.at(i).x;

			if (lastKeypointsLength > 0) {
				// toDo: check for min and max thresholds first
				double prev_x = (double)lastFramesKeypoints.at(i).x;
				avg += this_x - prev_x;
				numComparableKeypoints++;
			}
		}
		
		if (keypointsLength > 0) { // check for 0 to avoid illegal arithmetic operation
			avg = avg / numComparableKeypoints;

			if (lastKeypointsLength > 0) // check for 0 otherwise box will move right after doing second click
				human.move(avg, 0);
		}

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

vector<Point2i> RunningSpeedCalculator::findKeyPoints(Mat img) {
	// https://docs.opencv.org/2.4.13.2/modules/imgproc/doc/feature_detection.html#goodfeaturestotrack 
	vector< Point2i > corners;
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

void RunningSpeedCalculator::drawKeyPoints(Mat img, vector<Point2i> keypoints) {
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
	case EVENT_MBUTTONDOWN:
		human.move(-5, -5);
		break;
	default:
		break;
	}
}

void RunningSpeedCalculator::drawAreaOfInterest(Mat img, AreaOfInterest area) {
	rectangle(img, area.getPoint1(), area.getPoint2(), RED, 2);
}