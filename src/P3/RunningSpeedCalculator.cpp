#include "RunningSpeedCalculator.h"
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

RunningSpeedCalculator::~RunningSpeedCalculator() {
	// destructor
}

double RunningSpeedCalculator::process() {
	sequence->restart();

	Mat frame = sequence->nextFrame();
	vector<Point2i> lastFramesKeypoints;

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// process image
		Mat subImage = sequence->getSubImage(frame, human);
		vector<Point2i> keypoints = findKeyPoints(subImage);

		Point2i diff = compareKeypoints(keypoints, lastFramesKeypoints);
		human.move(diff.x, 0);

		// draw
		convertToBGRA(&frame);
		drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame, human);

		// display
		imshow("P3", frame);

		// set last frame keypoints to this frame keypoints before getting next frame
		lastFramesKeypoints = keypoints;
		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
			frame = sequence->nextFrame();
	}

	
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
	double minDistance = 5.;

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

Mat RunningSpeedCalculator::getFrameForSetup() {
	sequence->restart();
	Mat frame = sequence->nextFrame();
	drawAreaOfInterest(frame, human);
	sequence->restart();

	return frame;
}

void RunningSpeedCalculator::drawAreaOfInterest(Mat img, AreaOfInterest area) {
	rectangle(img, area.getPoint1(), area.getPoint2(), RED, 2);
}

Point2i RunningSpeedCalculator::compareKeypoints(vector<Point2i> thisFrame, vector<Point2i> lastFrame) {
	Point2i averageMovement(0, 0);
	vector<Point2i> diffs;

	// compare keypoints and move areaOfInterest
	int numComparableKeypoints = 0; // use this variable to calculate the average instead of dividing by keypointsLength which would take all elements into the calculation

		// store size locally to avoid a size() call every iteration of loop
	int keypointsLength = thisFrame.size();
	int lastKeypointsLength = lastFrame.size();

	//// iterate over keypoints
	for (size_t i = 0; i < keypointsLength; i++) {
		Point2i thisFramePoint = thisFrame.at(i);

		if (lastKeypointsLength > i && lastKeypointsLength > 0) { // to prevent errors
			Point2i lastFramePoint = lastFrame.at(i);
			double xdiff = thisFramePoint.x - lastFramePoint.x;
			double ydiff = thisFramePoint.y - lastFramePoint.y;

			diffs.push_back(Point2i(xdiff, ydiff));

			// ignore keypoints that have not moved
			if (xdiff != 0) {
				averageMovement.x += xdiff;
				numComparableKeypoints++;
			}
		}
	}

	//// filter out those keypoints that have moved a lot more than the others
	//for (size_t i = 0; i < diffs.size(); i++) {
	//	Point2i thisFramePoint = diffs.at(i);
	//	cout << thisFramePoint.x << "   ";
	//}

	//cout << endl;

	if (keypointsLength > 0 && lastKeypointsLength > 0 && numComparableKeypoints > 1) { // check for 0 to avoid illegal arithmetic operations
		averageMovement.x = averageMovement.x / numComparableKeypoints;
	} else {
		averageMovement.x = 0;
	}
	//cout << "average x: " << averageMovement.x << endl;

	return averageMovement;
}

/* algorithm for filtering out swapped points. practically useless algorithm which does not improve the algorithm */
//for (size_t j = 0; j < lastKeypointsLength; j++) {
//	Point2i point = lastFramesKeypoints.at(j).x;

//	if (point.x == thisFrame.x && keypoints[j].x == lastFrame.x && j != i	/*&&
//		point.y == thisFrame.y && keypoints[j].y == lastFrame.y*/				) {
//		cout << i << "-" << thisFrame.x << " match " << j << "-" << point.x << endl;
//		cout << i << "-" << thisFrame.y << " match " << j << "-" << point.y << endl;
//		swappedPoints = true;
//	}
//}