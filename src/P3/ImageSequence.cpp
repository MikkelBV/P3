#include "ImageSequence.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

ImageSequence::ImageSequence() {
	video = VideoCapture(0);
}

ImageSequence::ImageSequence(string path) {
	video = VideoCapture(path);
}

ImageSequence::~ImageSequence(){
	video.release();
}

Mat ImageSequence::nextFrame() {
	video >> currentFrame;
	return currentFrame;
}

Mat ImageSequence::getSubImage(Mat original, AreaOfInterest area) {
	Mat subImage;
	//old \/
	//Rect roi = Rect(area.getX(), area.getY(), area.getWidth(), area.getHeight());
	Rect roi = Rect(area.getPoint1(), area.getPoint2());
	original(roi).copyTo(subImage);

	return subImage;
}
