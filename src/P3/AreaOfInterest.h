#pragma once
#include <opencv2/opencv.hpp>

class AreaOfInterest {
private:
	bool firstClick=true;
	cv::Point2i point1 = cv::Point2i(0, 0); 
	cv::Point2i point2 = cv::Point2i(1, 1); 
	cv::Point2i size = cv::Point2i(0, 0);

public:
	void reset();
	void set(int x, int y);
	void move(int x, int y);
	cv::Point2i getPoint1();
	cv::Point2i getPoint2();
	cv::Point2i getSize();
	bool outOfBoundsOffset(int width, int height); // add to uml
	static const int SHAPESIZE = 2; // add to uml
};

