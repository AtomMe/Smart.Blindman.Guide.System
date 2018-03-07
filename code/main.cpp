/*
freeglutapp:free opengl
match:match points method (Calculate)
MatchPont:use match.cpp to compute match result
public.h:img_x,img_y
*/
//base headers
#include <iostream>
#include <cstdio>
#include <string.h>
#include <unistd.h>
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
//muti thread
#include <pthread.h>
//my own headers
#include "VideoCap.h"
#include "trafficlight.h"
#include "zebra.h"
#include "MatchPoint.h"
#include "freeglutapp.h"

using namespace std;
using namespace cv;

Mat tmpL,tmpL2,tmpR,GrayL,GrayR;
Mat out;
int timcnt0=0,timcnt1=0,timcnt2=0;
bool run_flag=true;
bool display_flag=false;
bool disp_change=false;
bool show_distance=true;
bool img_ready=false;
bool show_match=false;
bool en_zebra=true,en_light=true;
/*
void Put2Screen(float height[12][12])
{
	int i,j;
	for(i=0;i<24;i++)
	{
		printf("       ");
		for(j=0;j<12;j++)
		{
			if(i&0x01)
			{
				printf("|%5d ",(int)height[i/2][j]);
			}
			else
			{
				printf("-------");
			}
		}
		if(i&0x01) 
			putchar('|');
		else
			putchar('-');
		putchar('\n');
	}
}
*/
void *thread(void *ptr)
{
	//thread 1 code start
	Mat disp,v1_detect,v2_detect;
	while(run_flag==true)
	{

		float time1=static_cast<float>(getTickCount());
                if(img_ready==false) 
		{
			sleep(0.1);
			continue;
		}
		disp=tmpL.clone();
		v1_detect=GrayL.clone();
		v2_detect=tmpR.clone();
		if(en_light==true) 
			TrafficLight_Detect(&disp);//traffic light detect
		else
		{
			rectangle(disp,Rect(150,50,120,40),Scalar(0,0,0),-1,8,0);
			putText(disp, "Traffic:.",
					Point(20, 80),
					FONT_HERSHEY_COMPLEX, 1,
					Scalar(0, 0, 255),
					1, LINE_AA); 
		}
		if(en_zebra==true) 
			Zebra_Detect(&disp,&v1_detect);//zebra detect
		else
		{
			putText(disp, "Zabra:lost.",
					Point(20, 40),
					FONT_HERSHEY_COMPLEX, 1,
					Scalar(0, 0, 255),
					1, LINE_AA);		
		}
		if(show_distance==true)
		{
			putText(disp, "distance",
				Point(400, 70),
				FONT_HERSHEY_COMPLEX, 1,
				Scalar(255, 255, 255),
				3, LINE_AA); 
			putText(disp, "distance",
				Point(400, 70),
				FONT_HERSHEY_COMPLEX, 1,
				Scalar(0, 255, 0),
				1, LINE_AA); 
		}
		else
		{
			putText(disp, "intensity",
				Point(400, 70),
				FONT_HERSHEY_COMPLEX, 1,
				Scalar(255, 255, 255),
				3, LINE_AA); 
			putText(disp, "intensity",
				Point(400, 70),
				FONT_HERSHEY_COMPLEX, 1,
				Scalar(0, 255, 0),
				1, LINE_AA); 			
		}
		imshow("left",disp);
		imshow("right",v2_detect);
		if(show_match==true) imshow("match",out);
		timcnt1=(int)(((float)getTickCount()-time1)/getTickFrequency()*1000);
		char key=waitKey(50);
		switch(key)
		{
			case 'e':case 'E':
				if(show_distance==false)
				{
					show_distance=true;
					printf("Show distance!!\n");
				}
				else
				{
					show_distance=false;
					printf("Show electric shock intensity!!\n");
				}
				break;
			case 'z':case 'Z':
				if(en_zebra==true)
				{
					en_zebra=false;
					printf("disenable zabra detect!!\n");
				}
				else
				{
					en_zebra=true;
					printf("enable zabra detect!!\n");
				}
				break;
			case 'l':case 'L':
				if(en_light==true)
				{
					en_light=false;
					printf("disenable trafficlight detect!!\n");
				}
				else
				{
					en_light=true;
					printf("enable trafficlight detect!!\n");
				}
				break;
			case 'c':case 'C':
				disp_change=!disp_change;
				break;
			case 27:case 'q':case 'Q':
				run_flag=false;
				break;
			default:
				break;
		}
	}
	//thread 1 code end
    	run_flag=false;
	printf("thread 1 exit!!\n");
	return 0;
}
void *thread2(void *ptr)
{
	while(run_flag)
	{
		float time2=static_cast<float>(getTickCount());
		if(Video_Grap(&tmpL,&tmpR)==false)
		{
			printf("Capture imagine error!!\n");
            		img_ready=false;
			run_flag=false;
			return 0;
		}
		resize(tmpL,tmpL2,Size(),0.5,0.5,CV_INTER_LINEAR);
		cvtColor(tmpL2,GrayL,COLOR_BGR2GRAY);
		cvtColor(tmpR,GrayR,COLOR_BGR2GRAY);
		img_ready=true;
		timcnt2=(int)(((float)getTickCount()-time2)/getTickFrequency()*1000);
		sleep(0.05);
	}
	run_flag=false;
	printf("thread 2 exit!!\n");
	return 0;
}
int main(int argc, char *argv[])
{
	float deep[12][12];
	Mat v1_match,v2_match;
	int idL,idR;
	float time=static_cast<float>(getTickCount());
	printf("Please input the index of left camera:");
	scanf("%d",&idL);
	printf("Please input the index of right camera:");
	scanf("%d",&idR);
	if(VideoCap_Init(idL,idR)==false)
	{
		printf("Camera index is wrong!!\n");
		return -1;
	}
	Zabra_Init();//zabra detect init
	Match_Init();//match points method
	Comm_Init();//commumication
	Disp_Init(argc,argv);//free opengl

	namedWindow("left");//window init
	namedWindow("right");
#if SHOW_MATCH_OUT
	namedWindow("match");
#endif
	
	
	printf("Init need=%dms\nStart Muti thread application!!\n",(int)(((float)getTickCount()-time)/getTickFrequency()*1000));
	//muti thread
	pthread_t id,id2;
    	int ret = pthread_create(&id, NULL, thread, NULL);
	int ret1 = pthread_create(&id2, NULL, thread2, NULL);
    if(ret || ret1) 
    {
        cout << "Create pthread error!" << endl;
        return 1;
    }
    //thread 1 code start
	
	while(run_flag==true)
	{
		if(img_ready==false) 
		{
			sleep(0.1);
			continue;
		}
		float time0=static_cast<float>(getTickCount());
		v1_match=GrayL.clone();
		v2_match=GrayR.clone();

		Match_Point_Detect(&v1_match,&v2_match,&out,deep);
		Disp_Next(deep);
		/*
		int cnt=1;
		for(int i=0;i<12;i++)
		{
			for(int j=0;j<12;j++)
			{
				if(i%6 == 0)
				{s
					imgdata[i][j]=50000;
				}
				else
				{
					imgdata[i][j]=1;
				}
			}
		}
		*/
        Comm_Send_144(deep);
      //  Put2Screen(imgdata);
        timcnt0=(int)(((float)getTickCount()-time0)/getTickFrequency()*1000);
        printf("Run thread 0 need %dms,thread 1 need %dms,thread 2 need %dms\n",
		timcnt0,timcnt1,timcnt2);
		
	}

	//thread 1 code end
	Comm_Close();
	run_flag=false;
	printf("thread 0 exit!!\n");
	pthread_join(id, NULL);
    return 0;
}







