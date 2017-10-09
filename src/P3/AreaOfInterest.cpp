#include "AreaOfInterest.h"
#include <iostream>
using namespace std;

void AreaOfInterest::set(int mx, int my) {
	if (firstClick) {
		x = mx;
		y = my;
		cout << "hooman coords: " << x << ", " << y << endl;
		firstClick = false;
	}else {

		if (mx > x) {
			width = mx - x;
		} else {
			width = x - mx;
			x = mx;
		}

		if (my > y) {
			height = my - y;
		} else {
			height = y - my;
			y = my;
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
int AreaOfInterest::getX() { return x; }
int AreaOfInterest::getY() { return y; }
int AreaOfInterest::getWidth() { return width; }
int AreaOfInterest::getHeight() { return height; }