//
// Created by hp on 2018/11/17.
//

#ifndef II_BSOFT_MODULE5_H
#define II_BSOFT_MODULE5_H

#define M_PI 3.141592694

#include <iostream>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>

//#include <opencv/cxcore.h>
#include "mouse_callback.h"
#include "module2.h"
#include "module4.h"
#include "draw.h"

using namespace std;
using namespace cv;

double generateTheta(const Mat &src) {
	
    const double thres = 22.5;
    HeadRepresent = find_Color(src, HeadColor);
    TailRepresent = find_Color(src, TailColor);
    Point2f center = Point2f((HeadRepresent.x + TailRepresent.x) / 2,
                             (HeadRepresent.y + TailRepresent.y) / 2);
    Point2f destination = route.front();

    cout << " center = " << center << '\n';
    cout << " destination = " << destination << '\n';

    double cen_desDistance = distance<Point2f>(center, destination);
    if (cen_desDistance < thres) {
        ++passCnt;
        cout << "the route point " << passCnt << " is reached .\n";
		if (!route.empty()) {
			route.pop();
			if (route.empty())
				return -60000000;
				
			
		}
    }

    double thetaTH, thetaCD, deltaTheta;

    if (HeadRepresent.x == TailRepresent.x)
        thetaTH = (TailRepresent.y < HeadRepresent.y) ? 90 : -90;
    else
        thetaTH = 180 * atan2(HeadRepresent.y - TailRepresent.y,
                              HeadRepresent.x - TailRepresent.x) / M_PI;
    cout << "the angle of car = " << thetaTH << '\n';
    if (center.x == destination.x)
        thetaCD = (center.y < destination.y) ? 90 : -90;
    else
        thetaCD = 180 * atan2(destination.y - center.y,
                              destination.x - center.x) / M_PI;
    cout << "the angle of position = " << thetaCD << '\n';

    deltaTheta = thetaCD - thetaTH;
    if (deltaTheta >= 180)//360
        deltaTheta -= 360;
    else if (deltaTheta <= -180)
        deltaTheta += 360;
    cout << "delta theta = " << deltaTheta << '\n';
    return deltaTheta;
}

void generateCommand(double theta) {
    if (theta < thresAng && theta > -thresAng)
        cmd = FORWARD;
    else if (theta <= -thresAng && theta >= -180)
        cmd = LEFT;
    else if (theta >= thresAng && theta <= 180)
        cmd = RIGHT;
}



#endif //II_BSOFT_MODULE5_H
