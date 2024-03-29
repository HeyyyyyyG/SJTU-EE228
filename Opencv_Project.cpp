
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <cstdio>
#include <tchar.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>
#include <ctype.h>
#include <fstream>
#include "windows.h"
#include <cmath>
#include <cstring>
#include "global.h"
#include "module1.h"
#include "module2.h"
#include "module3.h"
#include "module4.h"
#include "module5.h"
#include "SerialPort.h"
//#include "SerialCommunication.h"


using namespace std;
using namespace cv;



//透视变换
CvPoint2D32f originpoints[4];    //保存四个点的原始坐标   
CvPoint2D32f newpoints[4];     //保存这四个点的新坐标 
int ranxy[4];   //线段端点的范围
int record_min[2];
int record_end[2];
//using namespace cv;
//using namespace std;

struct zPoint   //点
{
	int x;
	int y;
	zPoint() {}
	zPoint(int d1, int d2) { x = d1; y = d2; }
};

double pointDist(zPoint a, zPoint b)   //两点之间的距离
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void mouse(int mouseevent, int x, int y, int flags, void* param)   //响应鼠标单击
{
	int sx, sy;
	static int clickTimes = 0;

	if (mouseevent == CV_EVENT_LBUTTONDOWN)  	//按下左键 
	{
		if (clickTimes > 3)
		{
			cout << "Position: " << x << " " << y << endl;
			return;
		}
		sx = x; sy = y;  	 	 	 	 	//记录当前位置，作为拖动的起点
		originpoints[clickTimes] = cvPoint2D32f(sx, sy);
		cout << sx << "   " << sy << endl;
		clickTimes++;
		if (clickTimes == 4)
			cout << "请按任意键继续..." << endl;
	}

}

void mouse2(int mouseevent, int x, int y, int flags, void* param)
{
	static int clickTimes2 = 0;

	if (mouseevent == CV_EVENT_LBUTTONDOWN)  	//按下左键 
	{
		if (clickTimes2 > 3) return;
		if (clickTimes2 == 0)
		{
			ranxy[0] = x; ranxy[1] = y;  	 	 	 	 	//记录当前位置，作为拖动的起点
		}
		if (clickTimes2 == 1)
		{
			ranxy[2] = x; ranxy[3] = y;
		}
		if (clickTimes2 == 2)
		{
			record_min[0] = x;
			record_min[1] = y;
		}
		if (clickTimes2 == 3)
		{
			record_end[0] = x;
			record_end[0] = y;
		}
		cout << x << "   " << y << endl;
		clickTimes2++;
	}

}

//透视变换
template<class T> class Image {
private:
	IplImage * imgp;
public:
	Image(IplImage* img = 0) { imgp = img; }
	~Image() { imgp = 0; }
	inline T* operator[](const int rowIndx)
	{
		return ((T *)(imgp->imageData + rowIndx * imgp->widthStep));
	}
};
typedef struct {
	unsigned char b, g, r;
} RgbPixel;
typedef struct {
	unsigned char h, s, v;
} HsvPixel;
typedef struct {
	float h, s, v;
} HsvPixelFloat;
typedef Image<RgbPixel> RgbImage;
typedef Image<HsvPixelFloat>  HsvFloatImage;
typedef Image<HsvPixel>  HsvImage;
typedef Image<unsigned char> BwImage;

void cvThin(IplImage* src, IplImage* dst, int iterations = 1)   //细化函数
{
	cvCopy(src, dst);
	BwImage dstdat(dst);
	IplImage* t_image = cvCloneImage(src);
	BwImage t_dat(t_image);
	for (int n = 0; n < iterations; n++)
		for (int s = 0; s <= 1; s++)
		{
			cvCopy(dst, t_image);
			for (int i = 0; i < src->height; i++)
				for (int j = 0; j < src->width; j++)
					if (t_dat[i][j])
					{
						int a = 0, b = 0;
						int d[8][2] = { { -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 },{ 1, 0 },{ 1, -1 },{ 0, -1 },{ -1, -1 } };
						int p[8];
						p[0] = (i == 0) ? 0 : t_dat[i - 1][j];
						for (int k = 1; k <= 8; k++)
						{
							if (i + d[k % 8][0] < 0 || i + d[k % 8][0] >= src->height || j + d[k % 8][1] < 0 || j + d[k % 8][1] >= src->width)
								p[k % 8] = 0;
							else
								p[k % 8] = t_dat[i + d[k % 8][0]][j + d[k % 8][1]];
							if (p[k % 8])
							{
								b++;
								if (!p[k - 1]) a++;
							}
						}
						if (b >= 2 && b <= 6 && a == 1)
							if (!s && !(p[2] && p[4] && (p[0] || p[6])))
								dstdat[i][j] = 0;
							else if (s && !(p[0] && p[6] && (p[2] || p[4])))
								dstdat[i][j] = 0;
					}
		}
	cvReleaseImage(&t_image);
}

double pathdist(double x, double y, double x1, double y1, double x2, double y2) //点到直线的距离
{
	double dx = x2 - x1, dy = y2 - y1;
	double px = x - x1, py = y - y1;
	double prod = dx * px + dy * py;
	double len = sqrt(dx*dx + dy * dy);
	double len2 = sqrt(px*px + py * py);
	double s = prod / len;
	double ans = sqrt(len2*len2 - s * s);
	return ans;
}

///////////////蓝牙

HANDLE com = CreateFile(_T("COM6:"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);

void sendCommand(char c) {
	char data[2]; data[0] = c; data[1] = '\0';
	DWORD addr = 0;//将字符写入串口的地址，即将指令发送给蓝牙模块
	//cout << "chuanchu" << endl;
	if (!WriteFile(com, data, 1, &addr, 0))
		cout << "Send successfully" << endl;//如果没有连接模块则报错 
										  //以COM口为COM7为例，打开对应的串口 
	else
	{
		cout << "chenggong#######" << endl;
	}
}

double zArg(int x, int y)   //计算倾斜角
{
	return atan2(y, x) / 3.1415926535897 * 180;
}

double zAngle(zPoint head, zPoint tail, zPoint dest)   //计算小车与目标点的相对夹角
{
	double a1, a2, a;
	a1 = zArg(head.x - tail.x, head.y - tail.y);
	a2 = zArg(dest.x - tail.x, dest.y - tail.y);
	a = a2 - a1;
	if (a > 180) a -= 360;
	if (a < -180) a += 360;
	return a;
}

double dist(zPoint a, zPoint b)   //计算两点之间的距离
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
double maximum(double x, double y)
{
	return (x >= y) ? x : y;
}
double minimum(double x, double y)
{
	return (x <= y) ? x : y;
}
bool check_overlap(CvPoint*line, int x, int y)
{
	if (pathdist(x, y, line[0].x, line[0].y, line[1].x, line[1].y) < 30)
		if ((x <= maximum(line[0].x, line[1].x) || x >= minimum(line[0].x, line[1].x))||
			(y <= maximum(line[0].y, line[1].y) && y >= minimum(line[0].y, line[1].y)))
				return true;
	return false;
}
bool modify_overlap(CvPoint*line_i, CvPoint*line_j)
{
	bool flag1 = check_overlap(line_i, line_j[0].x, line_j[0].y);
	bool flag2 = check_overlap(line_i, line_j[1].x, line_j[1].y);
	cout << flag1 << flag2 << endl;
	if (flag1&&flag2)
	{
		
		line_j[0].x = -1000;
		line_j[0].y = -1000;
		line_j[1].x = -1000;
		line_j[1].y = -1000;
		return true;
	}
	return false;
}

int main()
{
	int dist_thres = 28;
	int angle_thres = 15;
	int sleep_time = 200;

	IplImage* dstimg2 = cvCreateImage(cvSize(400, 400), IPL_DEPTH_8U, 1);
	int counters = 0;
	cout << "kaishi" << endl;
	while (counters<1)
	{
		sendCommand('r');
		counters++;
		Sleep(300);
	}
	
	IplImage* img0;
	bool jump = false;
	VideoCapture capture(0);
	Mat frame;
	if (!capture.isOpened())
		return -1;

	//开始录像
	
	while (1) {
		capture >> frame; //获取一帧图像
		img0 = new IplImage(frame);
		cvShowImage("win0", img0);//win0用于获取第一帧图像用于后续处理
		if (cvWaitKey(30) >= 0) break; //等待1毫秒，如果期间按下任意键则退出循环
		delete img0;
	}
	
	cvDestroyWindow("win0");
	newpoints[0] = cvPoint2D32f(0, 0);//透视变换新的点的坐标设为窗口的四个顶点
	newpoints[1] = cvPoint2D32f(400, 0);
	newpoints[2] = cvPoint2D32f(0, 400);
	newpoints[3] = cvPoint2D32f(400, 400);

	img0 = new IplImage(frame);

	cvNamedWindow("win1");
	cvShowImage("win1", img0);//win1 显示原图像，并且用于选取四个点
	cvSetMouseCallback("win1", mouse);//设置回调函数


	cvWaitKey();//按键后变换图像

				//透视变换
	IplImage *target, *pre;
	IplImage* transimg = cvCreateImage(cvSize(400, 400), IPL_DEPTH_8U, 3);//创建一个400*400的24位彩色图像，保存变换结果 
	CvMat* transmat = cvCreateMat(3, 3, CV_32FC1);  //创建一个3*3的单通道32位  浮点矩阵保存变换数据
	cvGetPerspectiveTransform(originpoints, newpoints, transmat);  //要先根据四个点计算出变换矩阵，用来做透视变换 
	cvWarpPerspective(img0, transimg, transmat); //根据变换矩阵计算图像的透视变换 

												 //反色，以匹配细化函数的颜色
	CvSize pre_size = cvSize((*transimg).width, (*transimg).height);
	pre = cvCreateImage(pre_size, 8, 1);
	cvCvtColor(transimg, pre, CV_BGR2GRAY);//pre为灰度图
	cvShowImage("win2灰度图片", pre);

	//二值化
	CvSize target_size = cvSize((*pre).width, (*pre).height);
	target = cvCreateImage(target_size, IPL_DEPTH_8U, 1);
	cvThreshold(pre, target, 100, 255, CV_THRESH_BINARY_INV);//cvTHreshold用于将原来的灰度图变为二值化的图像
	cvShowImage("win3二值化", target);
	cvSmooth(target, dstimg2, CV_GAUSSIAN);
	target = dstimg2;
	//细化
	IplImage *thinImage;

	CvSize thinImage_size = cvSize((*target).width, (*target).height);
	thinImage = cvCreateImage(thinImage_size, IPL_DEPTH_8U, 1);
	cvThreshold(target, thinImage, 45, 255, CV_THRESH_BINARY_INV);
	cvThin(target, thinImage, 50);//细化图像
	cvShowImage("win4细化", thinImage);

	//Hough变换获取线段
	CvMemStorage* storage = cvCreateMemStorage();//创建一片内存区域存储线段数据 
	CvSeq* lines = cvHoughLines2(thinImage, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 30, 40, 40); //Hough变换找直线，存储在lines中间

																										  //画出线段
	CvSize img1_size = cvSize((*thinImage).width, (*thinImage).height);
	IplImage* img1 = cvCreateImage(img1_size, 32, 1);//新建一幅同样大的图像，用于画出找到的直线段
	cvSetZero(img1);//填充为黑色 

	vector<zPoint> pathtmp; //初始目标点
	vector<zPoint> pathpos; //整理后的目标点
							//设置目标点范围，设置ranxy
	cvSetMouseCallback("win4细化", mouse2);
	cvWaitKey();
	cvDestroyWindow("win1");
	cvDestroyWindow("win2灰度图片");
	cvDestroyWindow("win3二值化");
	for (int i = 0; i < 4; i++)
		cout << "线段范围" << ranxy[i] << ' ';
	cout << endl;

	//初始去重边
	for (int i = 0; i < lines->total; i++)
	{
		CvPoint* line_i = (CvPoint*)cvGetSeqElem(lines, i);
		if (line_i[0].x < 0 || line_i[0].y < 0 || line_i[1].x < 0 || line_i[1].y < 0)
			continue;
		for (int j = 0; j < lines->total; j++)
		{
			if (i == j)
				continue;
			CvPoint* line_j = (CvPoint*)cvGetSeqElem(lines, j);
			if (line_j[0].x < 0 || line_j[0].y < 0 || line_j[1].x < 0 || line_j[1].y < 0)
				continue;
			if(modify_overlap(line_i, line_j))
				cout<<i<<"is overlaped by"<<j<<endl;
		}
	}
	

	//将初始点的坐标存入pathtmp，并去除在边界外的点
	for (int i = 0; i < lines->total; i++)
	{   //line 用于存储两个端点
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i); //读取第i条线段的两个端点 
		pathtmp.push_back(zPoint(line[0].x, line[0].y));
		pathtmp.push_back(zPoint(line[1].x, line[1].y));
		cvLine(img1, line[0], line[1], cvScalar(255));  	//用白色画出这条线段 
		if (pathtmp[i * 2].x < ranxy[0] || pathtmp[i * 2].y<ranxy[1] || pathtmp[i * 2].x > ranxy[2] || pathtmp[i * 2].y>ranxy[3])
		{
			pathtmp[i * 2].x = pathtmp[i * 2].y = -1000;
			pathtmp[i * 2 + 1].x = pathtmp[i * 2 + 1].y = -1000;
			continue;
		}
		if (pathtmp[i * 2 + 1].x < ranxy[0] || pathtmp[i * 2 + 1].y<ranxy[1] || pathtmp[i * 2 + 1].x > ranxy[2] || pathtmp[i * 2 + 1].y>ranxy[3])
		{
			pathtmp[i * 2].x = pathtmp[i * 2].y = -1000;
			pathtmp[i * 2 - 1].x = pathtmp[i * 2 - 1].y = -1000;
			continue;
		}
	}

	//横纵坐标最小和最小的点作为第一个点
	int m = 100000, idx = 0;
	for (int i = 0; i < 2 * lines->total; i++)
	{
		int tmp = pathtmp[i].x + pathtmp[i].y;
		if (tmp < m && tmp>0)
		{
			idx = i;
			m = tmp;
		}
	}
	
	//加入前两个点
	pathpos.push_back(zPoint(pathtmp[idx].x, pathtmp[idx].y));
	pathtmp[idx].x = pathtmp[idx].y = -1000;
	if (idx % 2 == 0) idx++;
	else idx--;
	pathpos.push_back(zPoint(pathtmp[idx].x, pathtmp[idx].y));
	zPoint lastPoint = pathtmp[idx];
	pathtmp[idx].x = pathtmp[idx].y = -1000;


	//将整理点的顺序，并存入pathpos
	while (true)
	{
		//寻找与刚加入的点距离最短的点
		int m1 = 100000, idx1 = -1;
		for (int i = 0; i < 2 * lines->total; i++)
		{
			if (pathtmp[i].x < 0 || pathtmp[i].y < 0) continue;
			//if (pathtmp[i].x < ranxy[0] || pathtmp[i].y<ranxy[1]) continue;
			int tmp = pointDist(lastPoint, pathtmp[i]);
			//if (check_if_overlap(pathtmp, last_idx1, last_last_idx, i)) continue;
			if (tmp < m1)
			{
				idx1 = i;
				m1 = tmp;
			}
		}
		if (idx1 == -1) break; //所有点均已加入
		pathpos.push_back(zPoint(pathtmp[idx1].x, pathtmp[idx1].y));

		//去除重边
		for (int i = 0; i < 2 * lines->total; i++)
		{
			if (pathtmp[i].x < 0 || pathtmp[i].y < 0) continue;
			if (i == idx) continue;
			if (pointDist(pathtmp[idx], pathtmp[i]) < 60)
			{
				pathtmp[i].x = pathtmp[i].y = -1000;
				int j = (i % 2 == 0) ? i + 1 : i - 1;
				pathtmp[j].x = pathtmp[j].y = -1000;
			}
		}

		//加入另一个端点，并将这两点标记为已访问，-1表示已经访问过了
		pathtmp[idx1].x = pathtmp[idx1].y = -1000;
		if (idx1 % 2 == 0) idx1++;
		else idx1--;
		pathpos.push_back(zPoint(pathtmp[idx1].x, pathtmp[idx1].y));
		lastPoint = pathtmp[idx1];
		pathtmp[idx1].x = pathtmp[idx1].y = -1000;
	}

	//去除重叠的点
	for (int i = 0; i < pathpos.size(); ++i)
	{
		if (pathpos[i].x < 0)
			continue;
		if (i + 1 > pathpos.size() - 1)
			break;
		if (pointDist(pathpos[i], pathpos[i + 1]) < 35)
		{
			pathpos[i].x = (pathpos[i].x + pathpos[i + 1].x) / 2;
			pathpos[i].y = (pathpos[i].y + pathpos[i + 1].y) / 2;
			pathpos[i+1].x = -1000;
			pathpos[i+1].y = -1000;
		}
	}

	//滤除无效点
	vector<zPoint> pathtrans;
	cout << "还未去重目标点总数" << pathpos.size() << endl;
	for (int i = 0; i < pathpos.size(); ++i)
	{
		if (pathpos[i].x < 0)
			continue;
		pathtrans.push_back(zPoint(pathpos[i].x, pathpos[i].y));
	}
	pathpos = pathtrans;
	cout << "去重目标点总数" << pathpos.size()<<endl;

	//显示目标点
	for (int i = 0; i < pathpos.size(); i++)
	{
		cout << "目标点" << i << pathpos[i].x << " " << pathpos[i].y << endl;
		if (pathpos[i].x < 0)continue;
		cvCircle(img1, cvPoint(pathpos[i].x, pathpos[i].y), i + 1, CV_RGB(255, 255, 255), 4);
		waitKey();
		cvShowImage("win5显示目标点", img1);
	}
	cout << "finish half" << endl;

	//小车位置识别
	int currPoint = 0;
	double trn;
	IplImage* img3;
	IplImage *Frame;
	VideoCapture capture2(0);
	Mat frame2;
	if (!capture.isOpened())
		return -1;
	capture >> frame; //过滤缓冲区图像

	int mini = 1000000;
	for (int i = 0; i < pathpos.size(); ++i)
	{
		if (pointDist(zPoint(record_min[0], record_min[1]), zPoint(pathpos[i].x, pathpos[i].y)) < mini)
		{
			currPoint = i;
			mini = pointDist(zPoint(record_min[0], record_min[1]), zPoint(pathpos[i].x, pathpos[i].y));
		}
	}

	//识别小车位置
	waitKey();
	bool flag_extra = false;
	/*
	int minimum_one = 10000000;
	for (int i = 0; i < pathpos.size(); ++i)
	{
		if (pointDist(zPoint(record_min[0], record_min[1]), pathpos[i]) < minimum_one)
		{
			minimum_one = pointDist(zPoint(record_min[0], record_min[1]), pathpos[i]);
			currPoint = i;
		}
	}
	cout << "##############" << currPoint << "#############" << endl;
	cout << record_min[0] <<"       " <<record_min[1] << endl;
	waitKey();
	*/
	int last_one = currPoint;
	while (1) {
		capture >> frame;
		img3 = new IplImage(frame);
		Frame = img3;
		cvGetPerspectiveTransform(originpoints, newpoints, transmat);   //根据四个点计算变换矩阵

		CvSize transimg_size = cvSize((*transimg).width, (*transimg).height);
		IplImage *dst_image = cvCreateImage(transimg_size, 32, transimg->nChannels);
		IplImage *src_image_32 = cvCreateImage(transimg_size, 32, transimg->nChannels);

		HsvFloatImage HSVimg(dst_image);

		CvPoint2D32f Cred, Cblue;
		cvWarpPerspective(Frame, transimg, transmat);  //根据变换矩阵计算图像的变换
		cvConvertScale(transimg, src_image_32);//将原图转换为32f类型
		cvCvtColor(src_image_32, dst_image, CV_BGR2HSV);//得到HSV图，保存在dst_image中  参数范围H(0,360) S(0,1) V(0,255)

		int Nred = 0, Ngreen = 0;
		int sumx = 0, sumy = 0;

		for (int i = 0; i < 400; ++i) {               //寻找red中心
			for (int j = 0; j < 400; ++j) {
				if (((HSVimg[i][j].h > -1 && HSVimg[i][j].h < 20) || (HSVimg[i][j].h > 340 && HSVimg[i][j].h < 361)) && HSVimg[i][j].s > 0.3   && HSVimg[i][j].v > 100)
				{
					sumx += j; sumy += i; ++Nred;
				}
			}
		}
		if (Nred == 0)
			Nred = 1;
		Cred.x = sumx / Nred;
		Cred.y = sumy / Nred;

		sumx = 0; sumy = 0;                     //寻找green中心
		for (int i = 0; i < 400; ++i) {
			for (int j = 0; j < 400; ++j) {
				if ((HSVimg[i][j].h > 80 && HSVimg[i][j].h < 135) && HSVimg[i][j].s > 0.3  && HSVimg[i][j].v >= 60)
				{
					sumx += j; sumy += i; ++Ngreen;
				}
			}
		}
		if (Ngreen == 0)   
			Ngreen = 1;
		Cblue.x = sumx / Ngreen;
		Cblue.y = sumy / Ngreen;

		//画出两个中心
		cvCircle(transimg, cvPoint((int)(Cred.x), (int)(Cred.y)), 10, CV_RGB(255, 200, 200), 4);
		cvCircle(transimg, cvPoint((int)(Cblue.x), (int)(Cblue.y)), 10, CV_RGB(200, 200, 255), 4);
		//小车行进
		cvCircle(transimg, cvPoint((int)(pathpos[currPoint].x), (int)(pathpos[currPoint].y)), 10, CV_RGB(128, 128, 128), 4);//画出下一个路径点
		waitKey(5);
		cvShowImage("小车追踪win7", transimg);
		if (pointDist(zPoint(record_end[0], record_end[1]), zPoint((Cred.x + Cblue.x) / 2, (Cred.y + Cblue.y) / 2)) < dist_thres)
		{
			currPoint = last_one;
			cout << last_one<<endl;
			flag_extra=true;
			waitKey();

		}
		if (pointDist(zPoint((Cred.x + Cblue.x) / 2, (Cred.y + Cblue.y) / 2), pathpos[currPoint]) <dist_thres)
		{
			cout << "##############" << endl;
			cout << "reach point:  " << currPoint << endl;
			cout << "##############" << endl;
			if (jump||flag_extra)
				break;
			currPoint++;
			if (currPoint >= pathpos.size())
			{
				jump = true;
				currPoint = last_one;
			}
		}

		trn = zAngle(zPoint(Cred.x, Cred.y), zPoint(Cblue.x, Cblue.y), pathpos[currPoint]);
		cout << "car position: " << pointDist(zPoint(Cred.x, Cred.y), pathpos[currPoint]) << ' ' << trn <<" next point: " <<currPoint<<endl;
		//转向
		if (trn > angle_thres|| trn < (-1*angle_thres))
		{
			if (trn < 0)
			{
				sendCommand('l');
				cout << "send command l" << endl;
			}
			else
			{
				sendCommand('r');
				cout << "send command r" << endl;
			}
			Sleep(sleep_time);
			continue;
		}
		//前进
		if (pointDist(zPoint((Cred.x+Cblue.x)/2, (Cred.y+Cblue.y)/2), pathpos[currPoint]) > dist_thres)
		{
			sendCommand('f');
			cout << "send command F" << endl;
			Sleep(sleep_time);
			continue;
		}
	}
	delete img3; //释放内存		
	//cvWaitKey();

	return 0;
}
