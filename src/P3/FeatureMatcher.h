#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "AreaOfInterest.h"


using namespace cv;
using namespace std;

class FeatureMatcher {
public:
	FeatureMatcher();
	void compare(Mat img_obj, Mat *imb_scene);
	//Mat createObject(int x, int y);
private:
	Mat img_object;
	Mat img_scene;	
	Ptr<FeatureDetector> featureDetector;
	vector<KeyPoint> keypoints_object;
	vector<KeyPoint> keypoints_scene;
	Ptr<DescriptorExtractor> extractor;
	Mat descriptors_object;
	Mat descriptors_scene;
	Ptr<DescriptorMatcher> matcher;
	vector<DMatch> matches;
	vector<DMatch> good_matches;
	Mat img_matches;
	vector<Point2f> obj;
	vector<Point2f> scene;
};