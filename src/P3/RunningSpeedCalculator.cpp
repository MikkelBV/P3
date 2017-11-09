#include "RunningSpeedCalculator.h"
#include "BackgroundSubtraction.h"
#include "KFilter.h"
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

double RunningSpeedCalculator::process() {
	speed = 0; // what were trying to find

	sequence->restart();
	Mat frame = sequence->nextFrame();

	KFilter kf = KFilter(); //Kalman
	vector<Point2i> lastFramesKeypoints;
	BackgroundSubtraction bs = BackgroundSubtraction();
	boxOrigin = areaOfInterest.getPoint1();


	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		//convertToGreyscale(&frame);
		
		// histogram equalisation
		//equalizeHist(frame, frame);
		

		// background Subtraction
		//bs.track(&frame, &frame, areaOfInterest);
		// reduce noise
		//medianBlur(frame, frame, 7);

		// check if runner stopped running
		if (!stillRunning(frame))
			break;

		// process image
		//Mat subImage = sequence->getSubImage(frame, areaOfInterest);

		//vector<Point2i> keypoints = findKeyPoints(subImage);
		//Point2i diff = compareKeypoints(keypoints, lastFramesKeypoints);

		//KFilter
		kf.run(&frame);

		//areaOfInterest.move(diff.x, 0);
		
		// if not already running, check if running and set time stamp if true
		if (!isRunning) {
			runnerDidStart();
		}

		// draw
		//convertToBGRA(&frame);
		//drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame);

		// display
		cv::imshow("P3", frame);

		// set last frame keypoints to this frame keypoints before getting next frame
		//lastFramesKeypoints = keypoints;

		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
		{
			frame = sequence->nextFrame();
			frame = sequence->nextFrame();
			frame = sequence->nextFrame();
		}
	}
	
	return speed;
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
		corner.x += areaOfInterest.getPoint1().x;
		corner.y += areaOfInterest.getPoint1().y;
	}

	return corners;
}

void RunningSpeedCalculator::drawKeyPoints(Mat img, vector<Point2i> keypoints) {
	for (size_t i = 0; i < keypoints.size(); i++) {
		circle(img, keypoints[i], 6, BLUE, AreaOfInterest::SHAPESIZE);
	}
}

void RunningSpeedCalculator::onMouse(int x, int y, int event) {
	switch (event) {
	case EVENT_LBUTTONDOWN:
		areaOfInterest.set(x, y);
		break;
	case EVENT_RBUTTONDOWN:
		areaOfInterest.reset();
		break;
	case EVENT_MBUTTONDOWN:
		areaOfInterest.move(-5, -5);
		break;
	default:
		break;
	}
}

Mat RunningSpeedCalculator::getFrameForSetup() {
	sequence->restart();
	Mat frame = sequence->nextFrame();
	drawAreaOfInterest(frame);
	sequence->restart();

	return frame;
}

void RunningSpeedCalculator::drawAreaOfInterest(Mat img) {
	rectangle(img, areaOfInterest.getPoint1(), areaOfInterest.getPoint2(), RED, AreaOfInterest::SHAPESIZE);
}

Point2i RunningSpeedCalculator::compareKeypoints(vector<Point2i> thisFrame, vector<Point2i> lastFrame) {
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
		// averageMovement.x = averageMovement.x / keypointsLength;
	} else {
		averageMovement.x = 0;
	}

	return averageMovement;
}

// check if still running, and if not get time and set speed
bool RunningSpeedCalculator::stillRunning(Mat frame) {
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

// check if running, and if true set timestamp and isRunning
bool RunningSpeedCalculator::runnerDidStart() {
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