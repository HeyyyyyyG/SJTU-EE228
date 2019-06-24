//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MODULE1_H
#define II_BSOFT_MODULE1_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "mouse_callback.h"

#include "draw.h"

using namespace std;
using namespace cv;

void testVideo() {
    VideoCapture camera(0);
    if (!camera.isOpened()) {
        cout << "Open camera failed\n";
        return;
    }
    cout << "Open camera successfully\n";
    Mat img;
    while (1) {
        camera >> img;
        imshow("module1", img);
        if (waitKey(5) == 13)
            break;
    }
}

#endif //II_BSOFT_MODULE1_H
