#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <vector>
#include "EventHandler.h"
#include "RunningSpeedCalculator.h"


using namespace std;
using namespace cv;


int main(int argc, char* argv[]) {

	RunningSpeedCalculator *rsc = new RunningSpeedCalculator("test.mp4");
	//RunningSpeedCalculator *rsc = new RunningSpeedCalculator();

	double speed = rsc->process();

	cout << "Speed: " << speed << endl;
	system("pause");
	
	return 0;
}
