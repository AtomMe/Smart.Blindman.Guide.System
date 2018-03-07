#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "string.h"
#include "trafficlight.h"
#include "communicate.h"
#define PI 3.1415926575323846f
#define ComMin(d1,d2) (d1>d2? d2:d1)
#define GetMin(R,G,B) (ComMin(ComMin(R,G),B))
#define ComMax(d1,d2) (d1>d2? d1:d2)
#define GetMax(R,G,B) (ComMax(ComMax(R,G),B))

using namespace std;
using namespace cv;

bool red=false,green=false,yellow=false;

void ClearImg(Mat *img)
{
	int rows=img->rows;
	int cols=img->cols;
	for(int i=0;i<rows;i++)
	{
		uchar *p=img->ptr<uchar>(i);
		for(int j=0;j<cols;j++)
		{
			int r,g,b;
			b=(unsigned int)p[j*3+0];
			g=(unsigned int)p[j*3+1];
			r=(unsigned int)p[j*3+2];

			if(b+g+r>100)
			{
				/*p[j*3+0]=255;
				p[j*3+1]=255;
				p[j*3+2]=255;*/
			}
			else
			{
				p[j*3+0]=0;
				p[j*3+1]=0;
				p[j*3+2]=0;
			}

		}
	}
}

int detect(Mat *in)
{
	int h,s,v;//h(0~180) s(0~255) v(0~255)
	int rc=0,gc=0,yc=0;
	int res=0;
	for(int i=0;i<in->rows;i++)
	{
		uchar *p=in->ptr<uchar>(i);
		for(int j=0;j<in->cols;j++)
		{
			h=((unsigned int)p[j*3+0])*255/180;
			s=(unsigned int)p[j*3+1];
			v=(unsigned int)p[j*3+2];
			if(v>100 && s>100)
			{
				if(h>=0 && h<15 || h>=245)	rc++;
				if(h>=66 && h<130) 	gc++;
				if(h>=20 && h<65)	yc++;
			}
		}

	}
	int total=in->rows*in->cols;
	rc=rc*100/total;
	gc=gc*100/total;
	yc=yc*100/total;
	//printf("red=%3d,green=%3d,yellow=%3d\n",rc,gc,yc);
	
	if(rc>30 && gc<10 && yc<10) 
	{
		red=true;
		res++;
	}
	if(rc<10 && gc>30 && yc<10) 
	{
		green=true;
		res++;
	}
	if(rc<10 && gc<10 && yc>20) 
	{
		yellow=true;
		res++;
	}
	return res;
}
void TrafficLight_Detect(Mat *temp)
{
	Mat img,tmp=temp->clone();
	red=false;
	green=false;
	yellow=false;
	img=tmp.clone();
	ClearImg(&img);
	Mat res;
	cvtColor(img,res,COLOR_BGR2GRAY);
	GaussianBlur( res, res, Size(9, 9), 2, 2 );
	vector<Vec3f> circles;
	HoughCircles(res, circles, HOUGH_GRADIENT, 1, res.rows/10,
				 40, 40, 5, 80 // change the last two parameters
							// (min_radius & max_radius) to detect larger circles
				 );
	for( size_t i = 0; i < circles.size(); i++ )
	{
		Vec3i c = circles[i];
		//·¶Î§ÏÞÖÆ
		int x1,y1,x2,y2;
		x1=c[0]-c[2];
		y1=c[1]-c[2];
		x2=c[2]*2;
		y2=c[2]*2;
		if(x1<0) x1=0;
		if(y1<0) y1=0;
		if(x2+x1>tmp.rows) x2=tmp.cols-x1;
		if(y2+y1>tmp.rows) y2=tmp.rows-y1;
		//È¡¸ÐÐËÈ¤ÇøÓò
		Rect rect(x1,y1,x2,y2);  
		Mat ok;
		tmp(rect).copyTo(ok); 
       		 //imshow("detect",ok);
		cvtColor(ok,ok,COLOR_BGR2HSV);
		if(detect(&ok))
		{
			circle(*temp, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
		}
	}
	
	rectangle(*temp,Rect(150,50,120,40),Scalar(0,0,0),-1,8,0);
	if(red==false && green==false && yellow==false)
	{
		putText(*temp, "Traffic:",
			Point(20, 80),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(0, 0, 255),
			1, LINE_AA); 
	}
	else
	{
		putText(*temp, "Traffic:",
			Point(20, 80),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(255, 255, 255),
			3, LINE_AA); 
		putText(*temp, "Traffic:",
			Point(20, 80),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(0, 255, 0),
			1, LINE_AA); 
		if(red)
		{
			rectangle(*temp,Rect(165,55,30,30),Scalar(0,0,255),-1,8,0);
			Comm_SendVoice((char *)"���");
		}
		if(green)
		{
			rectangle(*temp,Rect(225,55,30,30),Scalar(0,255,0),-1,8,0);
			Comm_SendVoice((char *)"�̵�");
		}
		if(yellow)
		{
			rectangle(*temp,Rect(195,55,30,30),Scalar(0,255,255),-1,8,0);
			Comm_SendVoice((char *)"�Ƶ�");
		}
	}
//		imshow("Cal",res);
	//	imshow("img",tmp);
//		if(waitKey(100)==27) break;
}
