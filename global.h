//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_GLOBAL_H
#define II_BSOFT_GLOBAL_H

#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>

#include "SerialPort.h"

using namespace std;
using namespace cv;
//对小车的指令类型
enum COMMAND {
    FORWARD, LEFT, RIGHT, STOP
} cmd;

VideoCapture camera;



//像素点颜色R G B
struct Color {
    short R, G, B;

    Color(short r = 0, short g = 0, short b = 0) {
        R = r;
        G = g;
        B = b;
    }

    void show() const {
        cout << "( B : " << B << " G : " << G << " R : " << R << " )";
    }
};

//已排序的小车路径点队列
queue<Point2f> route;

//小车头尾的颜色
Color HeadColor, TailColor;

//车头车尾颜色的代表点
Point2f HeadRepresent, TailRepresent;

//控制选点MouseCallback函数选择车头还是车尾。1是车头，2是车尾,3是选择结束
short HeadOrTailMark = 1;

//选择4个透视变换点的计数器
short PerspectiveNum = 0;

//透视变换的变换矩阵
Mat H;

//透视变换4个点的目标坐标，也就是矩形图的四角坐标
vector<Point> PerspectiveDest(4);

//透视变换4个点的初始坐标，调用函数选取一次，以后不再改变
vector<Point2f> PerspectiveBase(4);

//是否添加了一个透视变换基点
bool addpoint;

//计算两个点之间的距离
template<class T>
inline double distance(T &p1, T &p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

//选择的路径点计数
short routeCnt = 0;

//通过的路径点计数
short passCnt = 0;

//发送指令的阈值角
short thresAng = 24;

Point2f temRoute;

//路径起点，用于自动寻路算法
cv::Point2f startpoint;

// 标记是否已经选取了路径开始点
bool getstart = 0;

//自动寻路算法的重要参数：判定两条直线距离的阈值，新路径的最短长度阈值
double disthres = 110, lenthres = 100;//110 and 100 for the small graph

//霍夫变换参数
int thre = 39, gmin = 23, gmax = 66;

//霍夫直线
vector<cv::Vec4i> lines;

//路径点向量，没有实际用途，只是用于绘图而已
vector<cv::Point2f> routepoints;

cv::Mat grayImg, thresImg, tem, pic;

#endif //II_BSOFT_GLOBAL_H
