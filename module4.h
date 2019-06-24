//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MODULE4_H
#define II_BSOFT_MODULE4_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>
#include "mouse_callback.h"
#include "draw.h"

using namespace std;
using namespace cv;


void set_HeadAndTailColor(Mat src) {
    namedWindow("module4", CV_WINDOW_NORMAL);
    setMouseCallback("module4", callback_FindPoint);
    cout << "Choose the car head and tail :\n";
    while (HeadOrTailMark < 3) {
        imshow("module4", src);
        waitKey(10);
    }
    waitKey(1000);
    destroyWindow("module4");
    cout << "The car head and tail are saved .\n";
    Vec3b temBGR;
    temBGR = src.at<Vec3b>((int) HeadRepresent.y, (int) HeadRepresent.x);
    HeadColor.B = temBGR.val[0];
    HeadColor.G = temBGR.val[1];
    HeadColor.R = temBGR.val[2];

    temBGR = src.at<Vec3b>((int) TailRepresent.y, (int) TailRepresent.x);
    TailColor.B = temBGR.val[0];
    TailColor.G = temBGR.val[1];
    TailColor.R = temBGR.val[2];

    cout << "The color of head = ";
    HeadColor.show();
    cout << "\nThe color of tail = ";
    TailColor.show();
    cout << "\nThe color of head and tail are saved .\n";

}

double ColorDifference(const Vec3b &x, const Color &col) {
    return sqrt((x[0] - col.B) * (x[0] - col.B) +
                (x[1] - col.G) * (x[1] - col.G) +
                (x[2] - col.R) * (x[2] - col.R)
    );
}

Point2f find_Color(const Mat &src, const Color &col) {
    int limit = 35;
    double addx = 0, addy = 0;
    int cnt = 0;
    for (int i = 0; i < src.rows; ++i) {
        auto *c = src.ptr<Vec3b>(i);
        for (int j = 0; j < src.cols; ++j) {
            if (limit > ColorDifference(c[j], col)) {
                addx += j;
                addy += i;
                ++cnt;
            }
        }
    }
    return Point2f((int) (addx / cnt), (int) (addy / cnt));
}

#endif //II_BSOFT_MODULE4_H
