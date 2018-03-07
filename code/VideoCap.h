#ifndef __VIDEOCAP_H
#define __VEDEOCAP_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "public.h"

using namespace cv;
bool VideoCap_Init(int no1,int no2);
bool Video_Grap(Mat *img1,Mat *img2);

#endif


