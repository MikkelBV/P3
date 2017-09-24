#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	// read the inputted file name (use default if none provided)
	string imgPath, defaultPath = "EISENHOWER.jpg";
	if (argc > 1) {
		imgPath = argv[1];
	} else {
		imgPath = defaultPath;
	}
	cout << "Loading file " << imgPath << "..." << endl;

	Mat image = imread(imgPath);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	imshow("image", image);
	waitKey(0);
	return 0;
}