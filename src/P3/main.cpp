#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Speeder.h"
#include <fstream>


using namespace std;
using namespace cv;

string chooseDefaultVideo(); // declare function before main() so we can call it in main(). Because c++ thats why
string chooseMethod(int input);
Speeder *rsc = NULL; 

// main.cpp will perform all the tasks that will be handled by Polaric later in project, mainly input
int main(int argc, char* argv[]) {
	string filePath = "";
	int method = 0, reps = 0, framesToSkip = 1;
	bool resizeVideo = false;

	switch (argc) {
	case 1:
		filePath = chooseDefaultVideo();
		break;
	case 2:
		filePath = argv[1];
		break;
	case 3:
		filePath = argv[1];
		method = stoi(argv[2]);
		break;
	case 6:
		filePath = argv[1];
		method = stoi(argv[2]);
		reps = stoi(argv[3]);
		if (reps < 1) {
			reps = 1;
		}
		resizeVideo = stoi(argv[4]);
		framesToSkip = stoi(argv[5]);
		break;
	default:
		cout << "Invalid arguments!" << endl;
		return -1;
		break;
	}

	cout << "Processing video: " << filePath << endl;
	cout << "Method: " << chooseMethod(method) << endl;
	cout << "Repetitions: " << reps << endl;
	cout << "Resize video before processing: " << (resizeVideo ? "yes" : "no") << endl;
	cout << "Skipping frames: " << framesToSkip << endl;
	cout << "----------------------------" << endl << endl;

	namedWindow("P3");
	moveWindow("P3", 0, 0);

	double avg = 0, min = 0, max = 0;

	for (int i = 0; i < reps; i++) {
		cout << endl;
		rsc = new Speeder(filePath/*, method*/);
		double speedCM = rsc->process(method, framesToSkip, resizeVideo);
		double speedKM = (speedCM / 100) * 3.6;
		avg += speedKM;
		if (speedKM < min || min == 0) min = speedKM;
		if (speedKM > max) max = speedKM;
	}

	if (reps == 0) {
		rsc = new Speeder(filePath);

		double speedCM = rsc->process(method, framesToSkip, resizeVideo);
		cout << "Speed: " << speedCM << " cm/sek" << endl;

		double speedKM = (speedCM / 100) * 3.6;
		cout << "Speed: " << speedKM << " km/h" << endl;
		// write output to file
		ofstream outputFile;
		outputFile.open("output.txt");
		outputFile << speedCM << "-cm/s-"; // writing done here // '-' used as seperator in server
		outputFile << speedKM << "-km/h"; // writing done here // '-' used as seperator in server
		outputFile.close();
	}
	else {
		avg = avg / reps;
		cout << "Test done" << endl;
		cout << "avg " << avg << " km/h" << endl;
		cout << "min " << min << " km/h" << endl;
		cout << "max " << max << " km/h" << endl;
	}


	destroyAllWindows();

	if (argc == 1) // dont pause if were running from server
		system("pause");

	return 0;
}

string chooseDefaultVideo() {
	string availableVideos[] = { "simple.mp4", "CarInBackground.mp4", "DistanceControl.mp4", "DrunkRun.mp4", "RedLight_Moving.mp4", "RedLight_Stationary.mp4", "RedLightBlink_StopInFrame.mp4", "Start_Outside_Frame.mp4", "Walk_Run_Walk.mp4", "Stop_In_Frame.mp4", "Stop_In_Frame_Continue.mp4", "Run_Stop.mp4", "sprint.mp4", "std_run_inside.mp4", "std_run_outside.mp4" };
	int numVideos = 15;

	string filePath = "videos/edited/";
	bool validPathChosen = false;

	while (!validPathChosen) {
		cout << "Select video: " << endl;
		for (int i = 0; i < numVideos; i++) {
			cout << i << ": " << availableVideos[i] << endl;
		}

		int answer;
		cin >> answer;

		if (answer >= 0 && answer < numVideos) {
			filePath += availableVideos[answer];
			validPathChosen = true;
		}
		else {
			system("cls");
			cout << "Invalid input!" << endl;
		}
	}

	return filePath;
}

string chooseMethod(int input) {
	switch (input) {
	case 0:
		return "Colour thresholding & Kalman filter";
	default:
		return "Unspecified";
	}
}