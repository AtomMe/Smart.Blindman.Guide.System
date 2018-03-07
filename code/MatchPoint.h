#ifndef __MATCHPOINT_H
#define __MATCHPOINT_H
#include "public.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
extern int nPoints;
extern float Mpoint[Img_X*Img_Y][3];

void Match_Init(void);
void Match_Point_Detect(Mat *img1,Mat *img2,Mat *out,float imgdata[12][12]);

#endif
