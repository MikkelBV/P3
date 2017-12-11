#include "MethodBlobDetection.h"
#include "BackgroundSubtraction.h"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

MethodBlobDetection::MethodBlobDetection() {
	sequence = new ImageSequence();
}

MethodBlobDetection::MethodBlobDetection(string path) {
	sequence = new ImageSequence(path);
}

double MethodBlobDetection::process() {
	speed = 0;  // what were trying to find

	setMouseCallback("P3", mouseHandler, this);
	sequence->restart();
	Mat frame = sequence->nextFrame();

	boxOrigin = areaOfInterest.getPoint1();
	vector<Point2i> lastFramesKeypoints;

	BackgroundSubtraction bs;

	blobDetector = setupBlobDetector();

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);

		// backgroundsubtraction & remove noise
		// BS creates a binary image on which the BLOB detector can be run on
		//bs.track(&frame, NULL, areaOfInterest); -- does not work as expected
		medianBlur(frame, frame, 7);

		// Check if runner stopped running
		if (!stillRunning(frame))
			break;

		// Process image
		Mat subImage = sequence->getSubImage(frame, areaOfInterest);

		// Blob detection on image in area of interest
		vector<cv::KeyPoint> keypoints;
		blobDetector->detect(subImage, keypoints);

		vector<Point2i> keypointsCasted;
		for (size_t i = 0; i < keypoints.size(); i++) {
			Point2f point = keypoints.at(i).pt;
			point.x += areaOfInterest.getPoint1().x;
			point.y += areaOfInterest.getPoint1().y;
			keypointsCasted.push_back(point);
		}

		Point2i diff = compareKeypoints(keypointsCasted, lastFramesKeypoints);
		lastFramesKeypoints = keypointsCasted;
		areaOfInterest.move(diff.x, 0);
		drawKeyPoints(frame, keypointsCasted);

		// center AOI around == keypoints[0].pt;

		// If not already running, check if running and set time stamp if true
		if (!isRunning) {
			runnerDidStart();
		}

		// Draw area of interest
		drawAreaOfInterest(frame);

		// Display result
		imshow("P3", frame);

		// Stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
			frame = sequence->nextFrame();
	}

	return speed;
}

void MethodBlobDetection::convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}

bool MethodBlobDetection::freezeAndWait(int ms) {
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

vector<Point2i> MethodBlobDetection::findKeyPoints(Mat img) {
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

void MethodBlobDetection::drawKeyPoints(Mat img, vector<Point2i> keypoints) {
	for (size_t i = 0; i < keypoints.size(); i++) {
		circle(img, keypoints[i], 6, BLUE, AreaOfInterest::SHAPESIZE);
	}
}

void MethodBlobDetection::onMouse(int x, int y, int event) {
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

void MethodBlobDetection::drawAreaOfInterest(Mat img) {
	rectangle(img, areaOfInterest.getPoint1(), areaOfInterest.getPoint2(), RED, AreaOfInterest::SHAPESIZE);
}

Point2i MethodBlobDetection::compareKeypoints(vector<Point2i> thisFrame, vector<Point2i> lastFrame) {
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
	if (keypointsLength > 0 && lastKeypointsLength > 0 && numComparableKeypoints > 0) {
		averageMovement.x = averageMovement.x / numComparableKeypoints;
		// averageMovement.x = averageMovement.x / keypointsLength;
	}
	else {
		averageMovement.x = 0;
	}
	cout << averageMovement << endl;
	return averageMovement;
}

// check if still running, and if not get time and set speed
bool MethodBlobDetection::stillRunning(Mat frame) {
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
bool MethodBlobDetection::runnerDidStart() {
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

// Set parameters for BLOB detector and creates it
Ptr<SimpleBlobDetector> MethodBlobDetection::setupBlobDetector() {
	SimpleBlobDetector::Params params;

	params.filterByArea = true;
	params.filterByCircularity = false;
	params.filterByConvexity = false;
	params.filterByInertia = false;
	params.filterByColor = true;
	params.minDistBetweenBlobs = 3000;
	params.blobColor = 255;

	return SimpleBlobDetector::create(params);
}