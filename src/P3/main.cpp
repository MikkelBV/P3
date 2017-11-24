#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "RunningSpeedCalculator.h"
#include <fstream>


using namespace std;
using namespace cv;

string chooseDefaultVideo(); // declare function before main() so we can call it in main(). Because c++ thats why
RunningSpeedCalculator *rsc = NULL; 

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

	double avg = 0, min = 0, max = 0;
	int reps = 30;

	for (int i = 0; i < reps; i++) {
		cout << endl;
		rsc = new RunningSpeedCalculator(filePath);
		double speedCM = rsc->process();
		double speedKM = (speedCM / 100) * 3.6;
		avg += speedKM;
		if (speedKM < min || min == 0) min = speedKM;
		if (speedKM > max) max = speedKM;
	}

	if (reps == 0) {
		rsc = new RunningSpeedCalculator(filePath);

		double speedCM = rsc->process();
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

	
	//writing the video and saving it on the computer
	int fourcc = CV_FOURCC('m', 'p', '4', 'v');
	const string filename = ("C:\processed_" + filePath);
	double fps = 10.0;
	Size frameSize;
	bool isColor;

	VideoWriter::VideoWriter(filename, fourcc, fps, frameSize, isColor = true);
	VideoCapture capName("filepath.format");
	Mat image;
	Mat outImage;

	int width = static_cast<int>(capName.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = static_cast<int>(capName.get(CV_CAP_PROP_FRAME_HEIGHT));

	VideoWriter writeName(filename, fourcc, fps, Size(width, height), false);
	while (true) {
		capName >> image;
		int jk = 1; // this should be replaced for the actual breakout condition
		if (!jk == 1) {
			break;
		}
		cout << image;
		imshow("window", image);
	}


	image.copyTo(outImage);
	cvtColor(outImage, outImage, CV_BGR2GRAY);
	Canny(outImage, outImage, 100, 300);
	cout << outImage;
	


	destroyAllWindows();

	if (argc == 1) // dont pause if were running from server
		system("pause");

	return 0;
}

string chooseDefaultVideo() {
	string availableVideos[] = { "simple.mp4", "CarInBackground.mp4", "DistanceControl.mp4", "DrunkRun.mp4", "RedLight_Moving.mp4", "RedLight_Stationary.mp4", "RedLightBlink_StopInFrame.mp4", "Start_Outside_Frame.mp4", "Walk_Run_Walk.mp4", "Stop_In_Frame.mp4", "Stop_In_Frame_Continue.mp4" };
	int numVideos = 11;

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

