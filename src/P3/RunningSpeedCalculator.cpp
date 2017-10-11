#include "RunningSpeedCalculator.h"
#include <iostream>

using namespace cv;
using namespace std;






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
	
	while (!frame.empty() ) {
		convertToGreyscale(&frame);

		Mat subImage = sequence->getSubImage(frame, human);
		vector<Point2f> keyPoints = findKeyPoints(subImage);
		
		convertToBGRA(&frame);
		drawKeyPoints(frame, keyPoints);
		drawAreaOfInterest(frame, human);

		imshow("subImage", subImage);
		imshow("P3", frame);

		// stop playing if user presses keyboard
		if (freezeAndWait(10))
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
	if (key == 32) {
		pausePlayback = !pausePlayback;
		return false;
	} else if (key > 0)
		return true;
	else
		return false;
}

vector<Point2f> RunningSpeedCalculator::findKeyPoints(Mat img) {
	vector< Point2f > corners;
	int maxCorners = 10;
	double qualityLevel = 0.01;
	double minDistance = 10.;

	goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance);

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
	rectangle(img, Point(area.getX(), area.getY()), Point(area.getX() + area.getWidth(), area.getY() + area.getHeight()), RED, 2);
}