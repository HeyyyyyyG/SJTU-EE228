//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MODULE3_H
#define II_BSOFT_MODULE3_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>
#include "mouse_callback.h"
#include "draw.h"
#include "CalculateGeomettry.h"

using namespace std;
using namespace cv;


//这是一种简单的手动取路径点的函数，用于调试硬软件交互模块
void SortedRoutePoints_naive(Mat &source) {
    cout << "naive find route points algorithm begins : \n";
    Mat src = source.clone();
    short storage = 0;
    namedWindow("module3", CV_WINDOW_NORMAL);
    setMouseCallback("module3", callback_FindRoute_naive);
    while (true) {
        imshow("module3", src);
        if (storage != routeCnt) {
            DrawPoint(src, temRoute);
            route.push(temRoute);
            storage = routeCnt;
        }
        if (waitKey(10) == 13)
            break;
    }
    destroyWindow("module3");
    cout << "Route saved .\n";
}
//这是自动选取路径点的函数，如果选取结果误差很大，则可以调用上面一种手动寻路函数
void SortedRoutePoints_automatic(cv::Mat &source) {
	cout << "automatic route points finding algorithm begins :\n";
	tem = source.clone();
	cv::namedWindow("module3", CV_WINDOW_NORMAL);
	cv::setMouseCallback("module3", callback_FindRoute_startpoint);
	while (true) {
		if (getstart)
			DrawPoint(tem, startpoint);
		imshow("module3", tem);
		if (cv::waitKey(10) == 13)
			break;
	}
	cv::destroyWindow("module3");

	tem = source.clone();
	int kx = 31, ky = 31, sx = 100, sy = 100;
	cv::GaussianBlur(tem, tem, CvSize(kx, ky), sx, sy);
	/*
		cv::namedWindow("module3", CV_WINDOW_NORMAL);
		while (true) {
			imshow("module3", tem);
			if (cv::waitKey(10) == 13)
				break;
		}
		cv::destroyWindow("module3");
	*/
	//--------------------------选择二值化参数---------------------
	int thres = 170;
	cv::cvtColor(tem, grayImg, cv::COLOR_BGR2GRAY);
	cv::namedWindow("module3", CV_WINDOW_NORMAL);
	cv::createTrackbar("threshold", "module3", &thres, 255, callback_Threshold);
	callback_Threshold(0, 0);
	cout << "Moditfiy thres\n";
	while (true) {
		cv::threshold(thresImg, tem, 100, 255, cv::THRESH_BINARY_INV);
		tem /= 240;
		tem = thinImage(tem, -1);
		tem *= 255;
		imshow("module3", tem);
		if (cv::waitKey(50) == 13)break;
	}
	cv::destroyAllWindows();
	//-----------------------------------------------------------


	//-----------------------不显示-------------------------------
		/*
	int thres = 103;
	cv::cvtColor(tem, grayImg, cv::COLOR_BGR2GRAY);
	cv::namedWindow("module3", CV_WINDOW_NORMAL);
	
	cv::threshold(grayImg, tem, thres, 255, cv::THRESH_BINARY_INV);
	tem /= 240;
	tem = thinImage(tem, -1);
	tem *= 255;
	cv::destroyWindow("module3");
	*/
	//-------------------------------------------------------------

	cv::namedWindow("module3", CV_WINDOW_NORMAL);
	cv::createTrackbar("thre", "module3", &thre, 100);
	cv::createTrackbar("gmin", "module3", &gmin, 100);
	cv::createTrackbar("gmax", "module3", &gmax, 100);
	cv::Mat plain(tem.size(), CV_8UC1);
	plain = 255 - plain;
	auto sto = tem.clone();
	tem = 255 - tem;
	while (true) {
		pic = plain.clone();
		cv::HoughLinesP(sto, lines, 1, CV_PI / 180, thre, gmin, gmax);
		DrawLines(pic, lines);
		imshow("module3", pic);
		if (cv::waitKey(10) == 13)
			break;
	}
	cv::destroyWindow("module3");


	FindRoute();
	cv::namedWindow("module3", CV_WINDOW_NORMAL);
	pic = source.clone();
	for (int i = 0; i < routepoints.size(); ++i) {
		printf("( %f , %f )\n", routepoints[i].x, routepoints[i].y);
		DrawPoint(pic, cv::Point(routepoints[i].x, routepoints[i].y));
	}

	while (true) {
		imshow("module3", pic);
		if (cv::waitKey(10) == 13)
			break;
	}
	cv::destroyWindow("module3");
}

#endif //II_BSOFT_MODULE3_H
