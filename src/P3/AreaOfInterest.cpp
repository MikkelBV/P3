#include "AreaOfInterest.h"
#include <iostream>
using namespace std;
using namespace cv;

void AreaOfInterest::set(int mx, int my) {
	if (firstClick) {
		point1 = Point2i(mx, my);
		point2 = Point2i(mx + 1, my + 1); // set the second coordinate to almost the same. This effectively resets the square
		cout << "Human area point 1: " << getPoint1() << endl;
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
		cout << "Human area size: " << getSize() << endl;

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

Point2i AreaOfInterest::getPoint1() { return point1; }
Point2i AreaOfInterest::getPoint2() { return point2; }
Point2i AreaOfInterest::getSize() { return size; }