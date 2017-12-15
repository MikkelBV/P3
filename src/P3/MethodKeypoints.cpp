#include "MethodKeypoints.h"
#include "BackgroundSubtraction.h"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

MethodKeypoints::MethodKeypoints() {
	sequence = new ImageSequence();
}

MethodKeypoints::MethodKeypoints(string path) {
	sequence = new ImageSequence(path);
}

double MethodKeypoints::process() {
	setMouseCallback("P3", MethodKeypoints:: mouseHandler, this);
	speed = 0; // What were trying to find

	sequence->restart();
	Mat frame = sequence->nextFrame();

	vector<Point2i> lastFramesKeypoints;
	boxOrigin = areaOfInterest.getPoint1();

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// Histogram equalisation
		equalizeHist(frame, frame);

		// Check if runner stopped running
		if (!stillRunning(frame))
			break;

		// Get image in area of interest
		Mat subImage = sequence->getSubImage(frame, areaOfInterest);

		vector<Point2i> keypoints = findKeyPoints(subImage);
		Point2i diff = compareKeypoints(keypoints, lastFramesKeypoints);

		areaOfInterest.move(diff.x, 0);

		// if not already running, check if running and set time stamp if true
		if (!isRunning) {
			runnerDidStart();
		}

		// Draw points and area of interest
		convertToBGRA(&frame);
		drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame);

		// Display
		cv::imshow("P3", frame);

		// Set last frame keypoints to this frame keypoints before getting next frame
		lastFramesKeypoints = keypoints;

		// Stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
			frame = sequence->nextFrame();
	}

	return speed;
}

void MethodKeypoints::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

void MethodKeypoints::convertToBGRA(Mat *img) {
	cvtColor(*img, *img, COLOR_GRAY2BGRA);
}

bool MethodKeypoints::freezeAndWait(int ms) {
	int key = waitKey(ms);

	if (key == 32) { // if keyboard input is space
		pausePlayback = !pausePlayback;
		return false;
	}
	else if (key == 8) { // if keyboard input is backspace
		sequence->restart();
		return false;
	}
	else if (key > 0) // if it was any other key
		return true;
	else // if no key was pressed
		return false;
}

vector<Point2i> MethodKeypoints::findKeyPoints(Mat img) {
	// https://docs.opencv.org/2.4.13.2/modules/imgproc/doc/feature_detection.html#goodfeaturestotrack 

	vector< Point2i > corners;
	int maxCorners = 5;
	double qualityLevel = 0.01;
	double minDistance = 5.;

	goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance);

	// Iterate over each keypoint and add the offset
	for (auto &corner : corners) {
		corner.x += areaOfInterest.getPoint1().x;
		corner.y += areaOfInterest.getPoint1().y;
	}

	return corners;
}

void MethodKeypoints::drawKeyPoints(Mat img, vector<Point2i> keypoints) {
	for (size_t i = 0; i < keypoints.size(); i++) {
		circle(img, keypoints[i], 6, BLUE, AreaOfInterest::SHAPESIZE);
	}
}

void MethodKeypoints::onMouse(int x, int y, int event) {
	switch (event) {
	case EVENT_LBUTTONDOWN: // Left button
		areaOfInterest.set(x, y);
		cout << "click ";
		break;
	case EVENT_RBUTTONDOWN: // Right button
		areaOfInterest.reset();
		break;
	case EVENT_MBUTTONDOWN: // ???
		areaOfInterest.move(-5, -5);
		break;
	default:
		break;
	}
}

void MethodKeypoints::drawAreaOfInterest(Mat img) {
	rectangle(img, areaOfInterest.getPoint1(), areaOfInterest.getPoint2(), RED, AreaOfInterest::SHAPESIZE);
}

Point2i MethodKeypoints::compareKeypoints(vector<Point2i> thisFrame, vector<Point2i> lastFrame) {
	Point2i averageMovement(0, 0);
	vector<Point2i> diffs;

	int numComparableKeypoints = 0; // use this variable to calculate the average instead of dividing by 
									// keypointsLength which would take all elements into the calculation

	// store size locally to avoid a size() call every iteration of loop
	int keypointsLength = thisFrame.size();
	int lastKeypointsLength = lastFrame.size();

	// iterate over keypoints
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

	// check for 0 to avoid illegal arithmetic operations
	if (keypointsLength > 0 && lastKeypointsLength > 0 && numComparableKeypoints > 1) {
		averageMovement.x = averageMovement.x / numComparableKeypoints;
	}
	else {
		averageMovement.x = 0;
	}

	return averageMovement;
}

// check if still running, and if not get time and set speed
bool MethodKeypoints::stillRunning(Mat frame) {
	if (areaOfInterest.outOfBoundsOffset(frame.cols, frame.rows)) {
		finishStamp = sequence->getTimeStamp();

		Point2i finalPosition = areaOfInterest.getPoint1();
		int pixelMovement = finalPosition.x - boxOrigin.x; // Get change in x position from origin to finish
		
		speed = pixelMovement / ((finishStamp - originStamp) / 1000);
		
		cout << "stop: " << finishStamp << " ms" << endl;
		cout << "distance: " << pixelMovement << " px" << endl;
		
		return false;
	}
	return true;
}

// check if running, and if true set timestamp and isRunning to true
bool MethodKeypoints::runnerDidStart() {
	Point2i currentPosition = areaOfInterest.getPoint1();
	int xdiff = currentPosition.x - boxOrigin.x;

	if (xdiff > RUNNING_MIN_THRESHHOLD) {
		isRunning = true;
		originStamp = sequence->getTimeStamp();
		cout << "start: " << originStamp << " ms" << endl;
		return true;
	}

	return false;
}