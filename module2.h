//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MODULE2_H
#define II_BSOFT_MODULE2_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
//#include <opencv/cxcore.h>
#include "mouse_callback.h"
#include "draw.h"

using namespace std;
using namespace cv;

void set_PerspectiveVec(Mat source) {
    Mat src = source.clone();
    PerspectiveDest[0] = Point(0, 0);
    PerspectiveDest[1] = Point(0, src.rows);
    PerspectiveDest[2] = Point(src.cols, src.rows);
    PerspectiveDest[3] = Point(src.cols, 0);
    namedWindow("module2", CV_WINDOW_NORMAL);
    setMouseCallback("module2", callback_PerspectiveTransform);
    cout << "Choose bases :\n";
    while (PerspectiveNum < 4) {
        if (addpoint) {
            addpoint = 0;
            DrawPoint(src, PerspectiveBase[PerspectiveNum - 1]);
        }
        imshow("module2", src);
        waitKey(10);
    }
    DrawPoint(src, PerspectiveBase[PerspectiveNum - 1]);
    imshow("module2", src);
    waitKey(1000);
    H = findHomography(PerspectiveBase, PerspectiveDest);
    cout << "Transform matrix generated .\n";
    destroyWindow("module2");
}

Mat PerspectiveTransform(Mat &src) {
    Mat ans;
    warpPerspective(src, ans, H, src.size());
    return ans;
}

#endif //II_BSOFT_MODULE2_H
