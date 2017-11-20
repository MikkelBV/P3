#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "AreaOfInterest.h"

using namespace cv;
using namespace std;

class FeatureTracker {
public:
	FeatureTracker();
	Mat createObject(int x, int y);
	void compare(Mat object, Mat scene);
private:
	Mat img_object;
	Mat img_scene;	
	FlannBasedMatcher matcher;
	FeatureDetector featureDetector;
	vector<KeyPoint> keypoints_object;
	vector<KeyPoint> keypoints_scene;
	DescriptorExtractor extractor;
	vector<DMatch> matches;
	vector<DMatch> good_matches;
	Mat img_matches;
};