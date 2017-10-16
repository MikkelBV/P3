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
	Mat frame = sequence->nextFrame();
	vector<Point2i> lastFramesKeypoints;

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// process image
		Mat subImage = sequence->getSubImage(frame, human);
		vector<Point2i> keypoints = findKeyPoints(subImage);
		vector<Point2i> filteredKeypointDifferences; // use these for feature tracking
		vector<Point2i> filteredKeypoints;
		
		// compare keypoints and move areaOfInterest
		double avg = 0;
		int numComparableKeypoints = 0; // use this variable to calculate the average instead of dividing by keypointsLength which would take all elements into the calculation

			// store size locally to avoid a size() call every iteration of loop
		int keypointsLength = keypoints.size();
		int lastKeypointsLength = lastFramesKeypoints.size(); 

			// iterate over keypoints. both keypoints vectors will always be the same size.
		for (size_t i = 0; i < keypointsLength; i++) { 
			Point2i thisFrame = keypoints.at(i);

			if (lastKeypointsLength > i && lastKeypointsLength > 0) {
				Point2i lastFrame = lastFramesKeypoints.at(i);

				double xdiff = thisFrame.x - lastFrame.x;
				double ydiff = thisFrame.y - lastFrame.y;

				// ignore keypoints that have not moved
				if (xdiff != 0) { // problem if only one point moves and it moves alot 
					avg += xdiff;
					numComparableKeypoints++;
				}
			}
		}

		cout << "------------" << endl;
		for (size_t i = 0; i < keypointsLength; i++) {
			Point2i thisFrame = keypoints.at(i);
			cout << thisFrame.x << "   ";
		}
		cout << endl;
		/*for (size_t i = 0; i < keypointsLength; i++) {
			Point2i thisFrame = keypoints.at(i);
			cout << thisFrame.y << "   ";
		}
		cout << endl;*/
		if (keypointsLength > 0 && lastKeypointsLength > 0 && numComparableKeypoints > 0) { // check for 0 to avoid illegal arithmetic operations
			avg = avg / numComparableKeypoints;
			human.move(avg, 0);
		}
		cout << "avg: " << avg << endl;

		// draw
		convertToBGRA(&frame);
		drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame, human);

		// display
		imshow("P3", frame);

		// set last frame keypoints to this frame keypoints before getting next frame
		lastFramesKeypoints = keypoints;

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(150))
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
	double qualityLevel = 0.1;
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