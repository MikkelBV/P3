#pragma once
#include <opencv2/opencv.hpp>

class AreaOfInterest {
private:
	bool firstClick=true;
	/*
	int x = 0, y = 0, width = 1, height = 1;
	int x2;
	int y2;
	*/

	//new -david
	cv::Point2i point1 = cv::Point2i(0, 0), point2 = cv::Point2i(1, 1), size = cv::Point2i(0, 0);

public:
	void reset();
	void set(int x, int y);
	/*
	int getX();
	int getY();
	int getWidth();
	int getHeight();
	*/

	//new -david
	cv::Point2i getPoint1(), getPoint2(), getSize();

};

