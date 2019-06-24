//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MOUSE_CALLBACK_H
#define II_BSOFT_MOUSE_CALLBACK_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>
#include "global.h"

using namespace cv;
using namespace std;


void callback_PerspectiveTransform(int event, int x, int y, int flags, void *) {
    if (PerspectiveNum < 4 && event == EVENT_LBUTTONDOWN) {
        cout << "No. " << PerspectiveNum + 1 << " base point = ( "
             << x << " , " << y << " )\n";
        PerspectiveBase[PerspectiveNum++] = Point2f(x, y);
        addpoint = true;
    }
}

void callback_FindPoint(int event, int x, int y, int flags, void *) {
    if (event == EVENT_LBUTTONDOWN && HeadOrTailMark == 1) {
        ++HeadOrTailMark;
        HeadRepresent = Point2f(x, y);
        cout << "the representative of head = ( " << x << " , " << y << " )\n";
    } else if (event == EVENT_LBUTTONDOWN && HeadOrTailMark == 2) {
        ++HeadOrTailMark;
        TailRepresent = Point2f(x, y);
        cout << "the representative of tail = ( " << x << " , " << y << " )\n";
    }
}

void callback_FindRoute_naive(int event, int x, int y, int flags, void *) {
    if (event == EVENT_LBUTTONDOWN) {
        ++routeCnt;
        cout << "naive find route : ( " << x << " , " << y << " )\n";
        temRoute = Point2f(x, y);
    }
}

void callback_FindRoute_startpoint(int event, int x, int y, int flags, void *) {
	if (getstart == 0 && event == cv::EVENT_LBUTTONDOWN) {
		getstart = 1;
		cout << "( " << x << " , " << y << " ) start point saved .\n";
		startpoint.x = x;
		startpoint.y = y;
	}
}
void callback_Threshold(int thre, void *) {
	cv::threshold(grayImg, thresImg, thre, 255, 0);
}

#endif //II_BSOFT_MOUSE_CALLBACK_H
