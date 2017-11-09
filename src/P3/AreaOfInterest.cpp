#include "AreaOfInterest.h"
#include <iostream>
using namespace std;
using namespace cv;

void AreaOfInterest::set(int mx, int my) {
	if (firstClick) {
		point1 = Point2i(mx, my);
		point2 = Point2i(mx + 1, my + 1); // set the second coordinate to almost the same. This effectively resets the square
		cout << "Area of interest point 1: " << point1 << endl;
		firstClick = false;
	} else {
		// check for possible negative size 
		if (mx < point1.x) {
			int temp = point1.x;
			point1.x = mx;
			mx = temp;
		}
		if (my < point1.y) {
			int temp = point1.y;
			point1.y = my;
			my = temp;
		}

		point2 = Point2i(mx, my);
		size = Point2i(point2.x - point1.x, point2.y - point1.y);
		cout << "Area of interest point 2: " << point2 << endl;
		cout << "Area of interest size: " << size << endl;

		firstClick = true;
	}
}

void AreaOfInterest::reset() {
	// set all variables to their initial value
	firstClick = true;
	point1 = Point2i(0, 0);
	point2 = Point2i(1, 1);
	size = Point2i(1, 1);
}

void AreaOfInterest::move(int x, int y) {
	point1.x += x;
	point2.x += x;

	point1.y += y;
	point2.y += y;
}

bool AreaOfInterest::outOfBoundsOffset(int width, int height) {
	Point2i diff;

	if (point1.x < 0 + SHAPESIZE) {
		diff.x = 0 - point1.x;
	} 
	else if (point2.x > width - SHAPESIZE) { // close program if runner left screen
		diff.x = width - point2.x;
		return true;
	}

	if (point1.y < 0 + SHAPESIZE) {
		diff.y = 0 - point1.y;
	}
	else if (point2.y > height - SHAPESIZE) { // close program if runner left screen
		diff.y = height - point2.y;
	}

	point1 += diff;
	point2 += diff;

	return false;
}

Point2i AreaOfInterest::getPoint1() { return point1; }
Point2i AreaOfInterest::getPoint2() { return point2; }
Point2i AreaOfInterest::getSize() { return size; }