#pragma once
#include <opencv2/opencv.hpp>

class AreaOfInterest {
private:
	//  When set-function is called, it is always first click
	bool firstClick = true;
	//  Top-left corner of area of interest
	cv::Point2i point1 = cv::Point2i(0, 0); 
	//  Bottom-right corner of area of interest
	cv::Point2i point2 = cv::Point2i(1, 1); 
	//  ???
	cv::Point2i size = cv::Point2i(0, 0);

public:
	//  Resets area of interest so that it can be redrawn
	void reset();
	//  Sets area of interest, with x and y as top-left corner
	void set(int x, int y);
	//  Moves area of interest, by x horizontally and y vertically 
	void move(int x, int y);
	//  Getters for points
	cv::Point2i getPoint1();
	cv::Point2i getPoint2();
	cv::Point2i getSize();
	//  Determines of the area of interest is out of bounds
	bool outOfBoundsOffset(int width, int height); 
	//  ???
	static const int SHAPESIZE = 2; 
};

