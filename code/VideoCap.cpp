#include "VideoCap.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;
VideoCapture v1,v2;

bool VideoCap_Init(int no1,int no2)
{
	v1.open(no1);
	v2.open(no2);
	if(!(v1.isOpened() && v2.isOpened()))
	{
		return false;
	}
	v2.set(CV_CAP_PROP_FRAME_WIDTH,Img_Y);
	v2.set(CV_CAP_PROP_FRAME_HEIGHT,Img_X);
	return true;
}
bool Video_Grap(Mat *img1,Mat *img2)
{
	v1>>*img1;
	v2>>*img2;
	if(img1->empty()||img2->empty()) 
	{
		cout<<"No Capture Data!!\n";
		return false;
	}
	return true;
}

