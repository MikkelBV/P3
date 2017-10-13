#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "RunningSpeedCalculator.h"


using namespace std;
using namespace cv;

RunningSpeedCalculator *rsc = NULL;

void mouseHandler(int event, int x, int y, int flags, void* userData) {
	rsc->onMouse(x, y, event);
}

int main(int argc, char* argv[]) {

	namedWindow("P3");
	namedWindow("subImage");
	setMouseCallback("P3", mouseHandler, NULL);

	// Retrieve path for video from command line. If no path is givenk it use the default test.mp4
	string inFile = "";
	if (argc == 2) {
		inFile = argv[1];
		cout << inFile << endl;
		rsc = new RunningSpeedCalculator(inFile);
	} else if (argc > 2) {
		cout << "Too many arguments" << endl;
	} else { // do shit in this thing (switch statement) files 0 - 7 (file_001)
		int ans=10;

		cout << "select the file you want to play" << endl;
		cin >> ans;
		switch (ans) {
		case 0:
			inFile = "file_000.mp4";
			break;

		case 1:
			inFile = "file_001.mp4";
			break;

		case 2:
			inFile = "file_002.mp4";
			break;

		case 3:
			inFile = "file_003.mp4";
			break;

		case 4:
			inFile = "file_004.mp4";
			break;

		case 5:
			inFile = "file_005.mp4";
			break;

		case 6:
			inFile = "file_006.mp4";
			break;

		case 7:
			inFile = "file_007.mp4";
			break;
			}
		//inFile = "test.mp4";
		cout << inFile << endl;
		rsc = new RunningSpeedCalculator(inFile);
	}

	double speed = rsc->process();
	cout << "Speed: " << speed << endl;

	system("pause");
	
	return 0;
}
