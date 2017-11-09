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
	cout << "Speed: " << speed << " px/sek" << endl;

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
		filePath = "video_original.mp4";
		break;
	case 1:
		filePath = "video_redball1.mp4";
		break;
	case 2:
		filePath = "video_redball2.mp4";
		break;
	case 3:
		filePath = "video_redball3.mp4";
		break;
	}

	return filePath;
}
