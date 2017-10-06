#pragma once
class AreaOfInterest {
public:
	bool firstClick=true;
	int x = NULL, y = NULL, width = NULL, height = NULL;

	void reset() {
		firstClick = true;
		x = NULL, y = NULL, width = NULL, height = NULL;

	}
};

