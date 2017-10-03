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
