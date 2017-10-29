#include "RunningSpeedCalculator.h"
#include "BackgroundSubtraction.h"
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
	double speed = 0; // what were trying to find

	sequence->restart();
	Mat frame = sequence->nextFrame();

	vector<Point2i> lastFramesKeypoints;
	BackgroundSubtraction bs = BackgroundSubtraction();
	Point2i boxOrigin = areaOfInterest.getPoint1();

	int originStamp= 0; // Timestamp when runner starts
	int finishStamp= 0; // Timestamp when runner finishes
	bool isRunning = false; // Boolean that checks movement(with threshhold)

	while (!frame.empty()) {
		// goodFeaturesToTrack() only works with 8 bit images
		convertToGreyscale(&frame);
		
		//Histogram equalisation
		equalizeHist(frame, frame);

		//Background Subtraction
		bs.track(&frame, &frame, areaOfInterest);
		medianBlur(frame, frame, 7);

		if (areaOfInterest.outOfBoundsOffset(frame.cols, frame.rows)) {	
			finishStamp = sequence->getTimeStamp();
			Point2i finalPosition = areaOfInterest.getPoint1();
			
			int pixelMovement = finalPosition.x - boxOrigin.x; // Get change in x position from origin to finish
			speed = pixelMovement / ((finishStamp - originStamp) / 1000);
			cout << "stop: " << finishStamp << " ms" << endl;
			cout << "distance: " << pixelMovement << " px" << endl;
			// output the calculated data
			//cout << "Person finishes: " << (finishStamp - originStamp) / 1000 << " Seconds later" << endl;
			//cout << "total Pixels moved: " << pixelMovement << endl;
			//cout << "Person ran " << pixelMovement / ((finishStamp - originStamp) / 1000) << " per second" << endl;
			break;
		}

		// process image
		Mat subImage = sequence->getSubImage(frame, areaOfInterest);

		vector<Point2i> keypoints = findKeyPoints(subImage);
		Point2i diff = compareKeypoints(keypoints, lastFramesKeypoints);
		
		areaOfInterest.move(diff.x, 0);
		
		// Check if runner has moved
		if (!isRunning) {
			//get current position of point1
			Point2i currentPosition = areaOfInterest.getPoint1();
			//compare position of point1 in each frame
			int xdiff = currentPosition.x - boxOrigin.x;
			if (xdiff > RUNNING_MIN_THRESHHOLD) {
				//set isRunning
				isRunning = true;
				originStamp = sequence->getTimeStamp();
				//Notify that person is running at what time(milliseconds)
				cout << "start: " << originStamp << " ms" << endl;
			}
		}

		// draw
		convertToBGRA(&frame);
		drawKeyPoints(frame, keypoints);
		drawAreaOfInterest(frame);

		// display
		cv::imshow("P3", frame);

		// set last frame keypoints to this frame keypoints before getting next frame
		lastFramesKeypoints = keypoints;
		// stop playing if user presses keyboard - wait for specified miliseconds
		if (freezeAndWait(40))
			break;
		else if (!pausePlayback)
			frame = sequence->nextFrame();
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
	int maxCorners = 15;
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

	if (keypointsLength > 0 && lastKeypointsLength > 0 && numComparableKeypoints > 1) { // check for 0 to avoid illegal arithmetic operations
		averageMovement.x = averageMovement.x / numComparableKeypoints;
	} else {
		averageMovement.x = 0;
	}

	return averageMovement;
}