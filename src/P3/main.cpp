#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <vector>

using namespace std;
using namespace cv;

// wrappers
Mat makeHistogram(Mat img) {
	//Make histograms
	int histSize = 256;

	//Ranges
	float range[] = { 0, 256 };
	const float* histRange = { range };

	Mat histogram;
	calcHist(&img, 1, 0, Mat(), histogram, 1, &histSize, &histRange, true, false);

	int hist_w = 600, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++) {
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histogram.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histogram.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	return histImage;
}
void convertToGreyscale(Mat *img) {
	cvtColor(*img, *img, COLOR_BGRA2GRAY);
}
void convertToGreyscale(Mat *img, Mat *target) {
	cvtColor(*img, *target, COLOR_BGRA2GRAY);
}

int main(int argc, char* argv[]) {
	VideoCapture video("test.mp4");
	//VideoCapture video(0);
	namedWindow("Simon");

	while (true) {
		Mat frame;
		video >> frame;
		convertToGreyscale(&frame);

		std::vector< cv::Point2f > corners;

		int maxCorners = 100;
		double qualityLevel = 0.01;
		double minDistance = 10.;

		goodFeaturesToTrack(frame, corners, maxCorners, qualityLevel, minDistance);

		for (size_t i = 0; i < corners.size(); i++) {
			cv::circle(frame, corners[i], 6, cv::Scalar(255.), 2);
		}

		imshow("Simon", frame);

		char key = waitKey(1);
		if (key > 0)
			break;
	}

	return 0;
}
