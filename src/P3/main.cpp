#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <vector>
#include "EventHandler.h"
#include "RunningSpeedCalculator.h"


using namespace std;
using namespace cv;

RunningSpeedCalculator *rsc = NULL;
//RunningSpeedCalculator *rsc = new RunningSpeedCalculator(); // 

void mouseHandler(int event, int x, int y, int flags, void* userData) {
	rsc->onMouse(x, y, event);
}

int main(int argc, char* argv[]) {

	namedWindow("P3");
	setMouseCallback("P3", mouseHandler, NULL);

	/*Retrieve path for video from command line
	* If no path is givenk it use the default test.mp4
	*/
	string inFile = "";
	if (argc == 2) {
			inFile = argv[1];
			cout << inFile << endl;
			rsc = new RunningSpeedCalculator(inFile);
	}
	else if (argc > 2)
	{
		cout << "Too many arguments" << endl;
	}
	else {
		inFile = "test.mp4";
		cout << inFile << endl;
		rsc = new RunningSpeedCalculator("test.mp4");
	}

	double speed = rsc->process();
	cout << "Speed: " << speed << endl;

	system("pause");
	
	return 0;
}
