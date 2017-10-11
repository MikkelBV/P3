#include "AreaOfInterest.h"
#include <iostream>
using namespace std;

void AreaOfInterest::set(int mx, int my) {
	if (firstClick) {
		/*
		x2 = mx;
		y2 = my;
		cout << "hooman coords: " << x << ", " << y << endl;
		firstClick = false;
		*/

		//new -david
		point1 = cv::Point2f(mx, my);
		cout << "Human area point 1: " << getPoint1();
		firstClick = false;


	}else {
		/*
		if (mx > x2) {
			width = mx - x2;
		} else {
			width = x2 - mx;
			x2 = mx;
		}

		if (my > y2) {
			height = my - y2;
		} else {
			height = y2 - my;
			y2 = my;
		}
		*/

		//new -david
		point2 = cv::Point2f(mx, my);
		cout << "Human area size: " << getSize() << endl;
		//cout << "hooman width and height: " << width << "px by " << height << "px" << endl;

	}
}

void AreaOfInterest::reset() {
	/*firstClick = true;
	x = 0;
	y = 0; 
	width = 1;
	height = 1;
*/

	//new -david
	firstClick = true;
	point1.x = 0;
	point1.y = 0;
	point2.x = 0;
	point2.y = 0;
	size.x = 0;
	size.y = 0;
	cout << "Area parameters has been reset" << endl;

}

/*
int AreaOfInterest::getX() { return x2; }
int AreaOfInterest::getY() { return y2; }
int AreaOfInterest::getWidth() { return width; }
int AreaOfInterest::getHeight() { return height; }
*/

//new -david
cv::Point2f AreaOfInterest::getPoint1() { return point1; }
cv::Point2f AreaOfInterest::getPoint2() { return point2; }
cv::Point2f AreaOfInterest::getSize() {
	
	if (point1.x > point2.x && point1.y > point2.y){
		size = cv::Point2f(point1.x - point2.x, point1.y - point2.y);
		return size;
	}
	else {
		size = cv::Point2f(point2.x - point1.x, point2.y - point1.y);
		return size;
	}
}