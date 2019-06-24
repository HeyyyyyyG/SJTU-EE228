//
// Created by hp on 2018/12/4.
//

#ifndef II_BSOFT_CALCULATEGEOMETTRY_H
#define II_BSOFT_CALCULATEGEOMETTRY_H
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv/cxcore.h>
#include "mouse_callback.h"
#include "draw.h"
#include "global.h"

double point_line_distance(cv::Point2i p, cv::Vec4i &l) {
    double A = l[3] - l[1], B = l[0] - l[2], C = l[2] * l[1] - l[3] * l[0];
    return abs(A * p.x + B * p.y + C) / sqrt(A * A + B * B);
}

double point_point_distance(cv::Point2f p1, cv::Point2f p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
                (p1.y - p2.y) * (p1.y - p2.y));
}

inline double Vec4i_length(cv::Vec4i &l) {
    return sqrt((l[3] - l[1]) * (l[3] - l[1]) +
                (l[2] - l[0]) * (l[2] - l[0]));
}

cv::Point2f getmit(cv::Vec4i &l1, cv::Vec4i &l2)
{
	double x1 = l1[0], y1 = l1[1], x2 = l1[2], y2 = l1[3];
	double x3 = l2[0], y3 = l2[1], x4 = l2[2], y4 = l2[3];
	cv::Point2f ans;
	if(abs(x1-x2)<0.5)
	{
		double k2 = (y3 - y4) / (x3 - x4);
		double b2 = (-x3 * y4 + x4 * y3) / (x4 - x3);
		ans.x = x1;
		ans.y = k2 * x1 + b2;
	}
	else if (abs(x3 - x4) < 0.5)
	{
		double k1 = (y1 - y2) / (x1 - x2);
		double b1 = b1 = (-x1 * y2 + x2 * y1) / (x2 - x1);
		ans.x = x3;
		ans.y = k1 * x3 + b1;
	}
	else
	{
		double k1 = (y1 - y2) / (x1 - x2), k2 = (y3 - y4) / (x3 - x4);
		double b1 = (-x1 * y2 + x2 * y1) / (x2 - x1), b2 = (-x3 * y4 + x4 * y3) / (x4 - x3);
		ans.x = (b2 - b1) / (k1 - k2);
		ans.y = (k1*b2 - k2 * b1) / (k1 - k2);
	}
	return ans;
}

cv::Mat thinImage(const cv::Mat &src, const int maxIterations) {
    assert(src.type() == CV_8UC1);
    cv::Mat dst;
    int width = src.cols;
    int height = src.rows;
    src.copyTo(dst);
    int count = 0;  // 记录迭代次数
    while (true) {
        count++;
        if (maxIterations != -1 && count > maxIterations) // 限制次数并且迭代次数到达
            break;
        std::vector<uchar *> mFlag; // 用于标记需要删除的点
        // 对点标记
        for (int i = 0; i < height; ++i) {
            uchar *p = dst.ptr<uchar>(i);
            for (int j = 0; j < width; ++j) {
                // 如果满足四个条件，进行标记
                // p9 p2 p3
                // p8 p1 p4
                // p7 p6 p5
                uchar p1 = p[j];
                if (p1 != 1) continue;
                uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
                uchar p8 = (j == 0) ? 0 : *(p + j - 1);
                uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
                uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
                uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
                uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
                uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
                uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
                if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6) {
                    int ap = 0;
                    if (p2 == 0 && p3 == 1) ++ap;
                    if (p3 == 0 && p4 == 1) ++ap;
                    if (p4 == 0 && p5 == 1) ++ap;
                    if (p5 == 0 && p6 == 1) ++ap;
                    if (p6 == 0 && p7 == 1) ++ap;
                    if (p7 == 0 && p8 == 1) ++ap;
                    if (p8 == 0 && p9 == 1) ++ap;
                    if (p9 == 0 && p2 == 1) ++ap;

                    if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0) {
                        //标记
                        mFlag.push_back(p + j);
                    }
                }
            }
        }

        // 将标记的点删除
        for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i) {
            **i = 0;
        }

        // 直到没有点满足，算法结束
        if (mFlag.empty()) {
            break;
        } else {
            mFlag.clear();// 将mFlag清空
        }

        // 对点标记
        for (int i = 0; i < height; ++i) {
            uchar *p = dst.ptr<uchar>(i);
            for (int j = 0; j < width; ++j) {
                // 如果满足四个条件，进行标记
                //  p9 p2 p3
                //  p8 p1 p4
                //  p7 p6 p5
                uchar p1 = p[j];
                if (p1 != 1) continue;
                uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
                uchar p8 = (j == 0) ? 0 : *(p + j - 1);
                uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
                uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
                uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
                uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
                uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
                uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

                if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6) {
                    int ap = 0;
                    if (p2 == 0 && p3 == 1) ++ap;
                    if (p3 == 0 && p4 == 1) ++ap;
                    if (p4 == 0 && p5 == 1) ++ap;
                    if (p5 == 0 && p6 == 1) ++ap;
                    if (p6 == 0 && p7 == 1) ++ap;
                    if (p7 == 0 && p8 == 1) ++ap;
                    if (p8 == 0 && p9 == 1) ++ap;
                    if (p9 == 0 && p2 == 1) ++ap;

                    if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0) {
                        //标记
                        mFlag.push_back(p + j);
                    }
                }
            }
        }

        // 将标记的点删除
        for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i) {
            **i = 0;
        }

        // 直到没有点满足，算法结束
        if (mFlag.empty()) {
            break;
        } else {
            mFlag.clear();//将mFlag清空
        }
    }
    return dst;
}

void FindRoute() {
    bool found;
    int t;
    vector<cv::Vec4i> kinds,sortedkinds;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i][3] < lines[i][1]) {
            t = lines[i][3];
            lines[i][3] = lines[i][1];
            lines[i][1] = t;
            t = lines[i][0];
            lines[i][0] = lines[i][2];
            lines[i][2] = t;
        }
        found = false;
        for (int j = 0; j < kinds.size(); ++j) {
            double cendis = point_point_distance(cv::Point2f((lines[i][0] + lines[i][2]) / 2,
                                                             (lines[i][1] + lines[i][3]) / 2),
                                                 cv::Point2f((kinds[j][0] + kinds[j][2]) / 2,
                                                             (kinds[j][1] + kinds[j][3]) / 2));
            double poithres = 1.8*point_point_distance(cv::Point2f(kinds[j][0],
                                                                   kinds[j][1]),
                                                       cv::Point2f(kinds[j][2],
                                                                   kinds[j][3]));//1.8 for the small graph
           
			
			if (cendis < poithres &&
                point_line_distance(cv::Point2i(lines[i][0], lines[i][1]), kinds[j]) +
                point_line_distance(cv::Point2i(lines[i][2], lines[i][3]), kinds[j]) < disthres) {
                found = true;
                if (lines[i][3] > kinds[j][3]) {
                    kinds[j][3] = lines[i][3];
                    kinds[j][2] = lines[i][2];
                }
                if (lines[i][1] < kinds[j][1]) {
                    kinds[j][1] = lines[i][1];
                    kinds[j][0] = lines[i][0];
                }
                break;
            }
        }
        if (!found && Vec4i_length(lines[i]) > lenthres) {
            kinds.push_back(lines[i]);
        }
    }

    pic = cv::Mat(tem.size(), CV_8UC1);
    pic = 255 - pic;
    DrawLines(pic, kinds);
    imshow("route", pic);
	int storage = kinds.size();
    cv::Point2f tempoint = startpoint,lastpoint;
    route.push(tempoint);
    routepoints.push_back(tempoint);
    vector<cv::Vec4i>::iterator kite;
	vector<cv::Vec4i> stor(kinds);
    while (!kinds.empty()) {

        int ind = 0;
        int ud = 0;//0 for the point with smaller y pos
        double val = point_point_distance(tempoint,
                                          cv::Point2f(kinds[0][0], kinds[0][1]));
        for (int i = 0; i < kinds.size(); ++i) {
            if (val > point_point_distance(
                    tempoint, cv::Point2f(kinds[i][0], kinds[i][1]))) {
                ind = i;
                ud = 0;
                val = point_point_distance(
                        tempoint, cv::Point2f(kinds[i][0], kinds[i][1]));
            }
            if (val > point_point_distance(
                    tempoint, cv::Point2f(kinds[i][2], kinds[i][3]))) {
                ind = i;
                ud = 1;
                val = point_point_distance(
                        tempoint, cv::Point2f(kinds[i][2], kinds[i][3]));
            }
        }
		sortedkinds.push_back(kinds[ind]);
        tempoint = (ud) ? cv::Point2f(kinds[ind][0], kinds[ind][1]) :
                   cv::Point2f(kinds[ind][2], kinds[ind][3]);

		if (kinds.size() == 1)lastpoint = tempoint;
        kite = kinds.begin();
        for (int t = 0; t < ind; ++t)
            ++kite;
        kinds.erase(kite);
		
        //route.push(tempoint);
        //routepoints.push_back(tempoint);
    }
	for (int i = 0; i < storage - 1; ++i)
		route.push(getmit(sortedkinds[i], sortedkinds[i + 1])),routepoints.push_back(getmit(sortedkinds[i], sortedkinds[i + 1]));
	route.push(lastpoint), routepoints.push_back(lastpoint);
}
/*void FindRoute() {
    bool found;
    int t;
    vector<cv::Vec4i> kinds,sortedkinds;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i][3] < lines[i][1]) {
            t = lines[i][3];
            lines[i][3] = lines[i][1];
            lines[i][1] = t;
            t = lines[i][0];
            lines[i][0] = lines[i][2];
            lines[i][2] = t;
        }
        found = false;
        for (int j = 0; j < kinds.size(); ++j) {
            double cendis = point_point_distance(cv::Point2f((lines[i][0] + lines[i][2]) / 2,
                                                             (lines[i][1] + lines[i][3]) / 2),
                                                 cv::Point2f((kinds[j][0] + kinds[j][2]) / 2,
                                                             (kinds[j][1] + kinds[j][3]) / 2));
            double poithres = 1.8*point_point_distance(cv::Point2f(kinds[j][0],
                                                                   kinds[j][1]),
                                                       cv::Point2f(kinds[j][2],
                                                                   kinds[j][3]));
            if (cendis < poithres &&
                point_line_distance(cv::Point2i(lines[i][0], lines[i][1]), kinds[j]) +
                point_line_distance(cv::Point2i(lines[i][2], lines[i][3]), kinds[j]) < disthres) {
                found = true;
                if (lines[i][3] > kinds[j][3]) {
                    kinds[j][3] = lines[i][3];
                    kinds[j][2] = lines[i][2];
                }
                if (lines[i][1] < kinds[j][1]) {
                    kinds[j][1] = lines[i][1];
                    kinds[j][0] = lines[i][0];
                }
                break;
            }
        }
        if (!found && Vec4i_length(lines[i]) > lenthres) {
            kinds.push_back(lines[i]);
        }
    }

    pic = cv::Mat(tem.size(), CV_8UC1);
    pic = 255 - pic;
    DrawLines(pic, kinds);
    imshow("route", pic);

    cv::Point2f tempoint = startpoint;
    route.push(tempoint);
    routepoints.push_back(tempoint);
    vector<cv::Vec4i>::iterator kite;
    while (!kinds.empty()) {

        int ind = 0;
        int ud = 0;//0 for the point with smaller y pos
        double val = point_point_distance(tempoint,
                                          cv::Point2f(kinds[0][0], kinds[0][1]));
        for (int i = 0; i < kinds.size(); ++i) {
            if (val > point_point_distance(
                    tempoint, cv::Point2f(kinds[i][0], kinds[i][1]))) {
                ind = i;
                ud = 0;
                val = point_point_distance(
                        tempoint, cv::Point2f(kinds[i][0], kinds[i][1]));
            }
            if (val > point_point_distance(
                    tempoint, cv::Point2f(kinds[i][2], kinds[i][3]))) {
                ind = i;
                ud = 1;
                val = point_point_distance(
                        tempoint, cv::Point2f(kinds[i][2], kinds[i][3]));
            }
        }
        tempoint = (ud) ? cv::Point2f(kinds[ind][0], kinds[ind][1]) :
                   cv::Point2f(kinds[ind][2], kinds[ind][3]);
        kite = kinds.begin();
        for (int t = 0; t < ind; ++t)
            ++kite;
        kinds.erase(kite);
        route.push(tempoint);
        routepoints.push_back(tempoint);
    }
}
*/
#endif //II_BSOFT_CALCULATEGEOMETTRY_H
