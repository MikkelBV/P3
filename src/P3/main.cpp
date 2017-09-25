#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	Mat edges;
	namedWindow("video_t", 1);
	namedWindow("video", 1);

	while (true) {
		Mat frame, tframe, hsv;
		cap >> frame; 
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		Scalar lower_red_l = Scalar(0, 0, 0);
		Scalar lower_red_h = Scalar(150, 150, 150);
		inRange(hsv, lower_red_l, lower_red_h, tframe);
		imshow("video_t", tframe);
		imshow("video", frame);
		waitKey(50);
	}
	return 0;
}