#ifndef BOX_PAINTING_H 
#define BOX_PAINTING_H 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

Mat paint_box(Mat img, double min_area_threshold, vector<vector<Point>> contours, float overlappingRate);

#endif