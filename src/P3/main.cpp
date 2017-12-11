#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Speeder.h"
#include <fstream>


using namespace std;
using namespace cv;

string chooseDefaultVideo(); 
string printMethod(int input);
Speeder *rsc = NULL; 

int main(int argc, char* argv[]) {

	// these variables can be set by cmd line arguments
	string filePath = ""; // video to be processed
	int method = METHOD_BACKGROUNDSUBTRACTION; // specifies which approach to process the video with
	int reps = 1; // specifies how many times the video should be processed (more reps -> more accurate result -> slower processing)
	int framesToSkip = 1; // specifies how many frames to skip after processing a frame. 1 is realtime and also the minimumvalue (more frames skipped -> less accurate result -> faster processing)
	bool resizeVideo = false; // specifies whether a frame should be resized before processing

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
				reps = 1; // a min of 1 rep required
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
	cout << "Method: " << printMethod(method) << endl;
	cout << "Repetitions: " << reps << endl;
	cout << "Resize video before processing: " << (resizeVideo ? "yes" : "no") << endl;
	cout << "Skipping frames: " << framesToSkip << endl;
	cout << "----------------------------" << endl << endl;

	namedWindow("P3");
	moveWindow("P3", 0, 0);

	double avg = 0, min = 0, max = 0, sd = 0;
	vector<int> sdVector;

	for (int i = 0; i < reps; i++) {
		cout << endl;
		rsc = new Speeder(filePath);
		double speedCM = rsc->process(method, framesToSkip, resizeVideo);
		double speedKM = (speedCM / 100) * 3.6;
		avg += speedKM;
		sdVector.push_back(speedKM);
		if (speedKM < min || min == 0) min = speedKM;
		if (speedKM > max) max = speedKM;	
	}
	avg = avg / reps;
	
	for (size_t i = 0; i < sdVector.size(); i++) {
		sd += pow(sdVector[i] - avg, 2);
	}
	sd = sqrt(sd / reps);
	

	cout	<< "avg: " << avg << endl 
			<< "min: " << min << endl 
			<< "max: " << max << endl 
			<< "sd: " << sd << endl;

	ofstream outputFile;
	outputFile.open("output.txt");
	outputFile << avg << "/"; // writing done here // '-' used as seperator in server
	outputFile << min << "/";
	outputFile << max << "/";
	outputFile << sd;
	outputFile.close();

	destroyAllWindows();

	if (argc == 1) // dont pause if were running from server
		system("pause");

	return 0;
}

string chooseDefaultVideo() {
	string availableVideos[] =	{	"9m_jog.mp4",
									"9m_occluded.mp4",
									"9m_sprint.mp4",
									"20m_jog.mp4",
									"20m_sprint.mp4",
									"stopinsideframe.mp4",
									"video_keypointmethod.mp4"
								};
	int numVideos = 7;

	string filePath = "videos/";
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

string printMethod(int input) {
	switch (input) {
	case METHOD_KALMAN:
		return "Colour thresholding & Kalman filter";
	case METHOD_BACKGROUNDSUBTRACTION:
		return "Backgroundsubtraction";
	case METHOD_BLOBDETECTION:
		return "Blob detection";
	case METHOD_KEYPOINTS:
		return "Keypoints tracking";
	default:
		return "Unspecified";
	}
}