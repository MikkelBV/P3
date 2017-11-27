#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "RunningSpeedCalculator.h"


using namespace std;
using namespace cv;

string chooseDefaultVideo(); // declare function before main() so we can call it in main(). Because c++ thats why
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
		return -1; // exit
	} else { 
		filePath = chooseDefaultVideo();
	}

	cout << "Video: " << filePath << endl;

	namedWindow("P3");
	moveWindow("P3", 0, 0);
	setMouseCallback("P3", mouseHandler, NULL);
	rsc = new RunningSpeedCalculator(filePath);

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

string chooseDefaultVideo() {
	string availableVideos[] = { "video_redball1.mp4" , "video_redball2.mp4" , "video_redball3.mp4" };
	int numVideos = 3;

	string filePath = "";
	bool validPathChosen = false;

	while (!validPathChosen) {
		cout << "Select video: " << endl;
		for (int i = 0; i < numVideos; i++) {
			cout << i << ": " << availableVideos[i] << endl;
		}

		int answer;
		cin >> answer;

		if (answer >= 0 && answer < numVideos) {
			filePath = availableVideos[answer];
			validPathChosen = true;
		}
		else {
			system("cls");
			cout << "Invalid input!" << endl;
		}
	}

	return filePath;
}
