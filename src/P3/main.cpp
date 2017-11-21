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

	rsc = new RunningSpeedCalculator(filePath);

	/*double speedCM = rsc->process();
	cout << "Speed: " << speedCM << " cm/sek" << endl;

	double speedKM = (speedCM / 100) * 3.6;
	cout << "Speed: " << speedKM << " km/h" << endl;
	*/
	// write output to file
	ofstream outputFile;
	outputFile.open("output.txt");
	//outputFile << speedCM << " cm/s, "; // writing done here
	//outputFile << speedKM << " km/h"; // writing done here
	outputFile.close();

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
