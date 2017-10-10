#pragma once
class AreaOfInterest {
private:
	bool firstClick=true;
	int x = 0, y = 0, width = 1, height = 1;
	int x2;
	int y2;

public:
	void reset();
	void set(int x, int y);
	int getX();
	int getY();
	int getWidth();
	int getHeight();

};

