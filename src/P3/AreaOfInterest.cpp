#include "AreaOfInterest.h"
#include <iostream>
using namespace std;

void AreaOfInterest::set(int mx, int my) {
	if (firstClick) {
		x2 = mx;
		y2 = my;
		cout << "hooman coords: " << x << ", " << y << endl;
		firstClick = false;
	}else {

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

		cout << "hooman width and height: " << width << "px by " << height << "px" << endl;
	}
}

void AreaOfInterest::reset() {
	firstClick = true;
	x = 0;
	y = 0; 
	width = 1;
	height = 1;
}
int AreaOfInterest::getX() { return x2; }
int AreaOfInterest::getY() { return y2; }
int AreaOfInterest::getWidth() { return width; }
int AreaOfInterest::getHeight() { return height; }