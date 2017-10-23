#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "RunningSpeedCalculator.h"


using namespace std;
using namespace cv;

string chooseVideo(); // declare function before main() so we can call it in main(). Because c++ thats why
RunningSpeedCalculator *rsc = NULL;

void mouseHandler(int event, int x, int y, int flags, void* userData) {
	rsc->onMouse(x, y, event);
}

// main.cpp will perform all the tasks that will be handled by Polaric later in project, mainly input
int main(int argc, char* argv[]) {

	// Retrieve path for video from command line. If no path is given ask the user
	string filePath = "";

	if (argc == 2) {
		filePath = argv[1];
	} else if (argc > 2) {
		cout << "Too many arguments" << endl;
		return 0; // exit
	} else { 
		filePath = chooseVideo();
	}

	cout << "Video: " << filePath << endl;

	namedWindow("P3");
	moveWindow("P3", 0, 0);
	setMouseCallback("P3", mouseHandler, NULL);

	rsc = new RunningSpeedCalculator(filePath);
	cout << "Mark the area of interest and press any key to start processing the video" << endl;

	Mat firstFrame = rsc->getFrameForSetup();
	while (waitKey(40) < 0) { // continue when key is pressed
		imshow("P3", firstFrame);
		firstFrame = rsc->getFrameForSetup(); // keep redrawing same frame but reload it
	}

	double speed = rsc->process();
	double speed2 = rsc->process();
	cout << "Speed: " << speed << endl;

	destroyAllWindows();
	system("pause");
	
	return 0;
}

string chooseVideo() {
	int ans;
	string filePath = "";
	cout << "Select video 0 - 8: " << endl;

	cin >> ans;
	switch (ans) {
	case 0:
		filePath = "file_000.mp4";
		break;
	case 1:
		filePath = "file_001.mp4";
		break;
	case 2:
		filePath = "file_002.mp4";
		break;
	case 3:
		filePath = "file_003.mp4";
		break;
	case 4:
		filePath = "file_004.mp4";
		break;
	case 5:
		filePath = "file_005.mp4";
		break;
	case 6:
		filePath = "file_006.mp4";
		break;
	case 7:
		filePath = "file_007.mp4";
		break;
	case 8:
		filePath = "test.mp4";
		break;
	}

	return filePath;
}
