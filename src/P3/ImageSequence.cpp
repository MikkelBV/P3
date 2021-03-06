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

Mat ImageSequence::nextFrame(int framesToSkip) {
	for (int i = 0; i < framesToSkip; i++)
		video >> currentFrame;

	return currentFrame;
}

Mat ImageSequence::getSubImage(Mat inputMat, AreaOfInterest area) {
	Mat subImage;
	Rect roi = Rect(area.getPoint1(), area.getPoint2());
	inputMat(roi).copyTo(subImage);

	return subImage;
}

void ImageSequence::restart(){
	video.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
}

VideoCapture* ImageSequence::getVideo() {
	return &video;
}

int ImageSequence::getTimeStamp() {
	int timeStamp = video.get(CAP_PROP_POS_MSEC);
	return timeStamp;
}