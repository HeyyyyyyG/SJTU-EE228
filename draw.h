//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_DRAW_H
#define II_BSOFT_DRAW_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv/cxcore.h>
#include "mouse_callback.h"

using namespace std;
using namespace cv;

/*inline void DrawPoint(Mat img, Point p) {
    circle(img, p, img.cols / 128, Scalar(0, 255, 0), -1, 8);
}*/
inline void DrawPoint_blue(Mat img, Point p) {
	circle(img, p, img.cols / 128, Scalar(0, 0, 255), -1, 8);
}

// new version  12/7
inline void DrawPoint(cv::Mat img, cv::Point p) {
	circle(img, p, img.cols / 128, cv::Scalar(0, 255, 0), -1, 8);
}

void DrawLine(cv::Mat img, cv::Point start, cv::Point end) {
	int thickness = 1;
	int lineType = 8;
	line(img, start, end, cv::Scalar(100, 100, 100), 1, CV_AA);
}

void DrawLines(cv::Mat img, vector<cv::Vec4i> lines) {

	for (size_t i = 0; i < lines.size(); ++i) {
		cv::Vec4i l = lines[i];
		DrawLine(img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]));
	}
}

#endif //II_BSOFT_DRAW_H
