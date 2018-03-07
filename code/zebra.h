#ifndef __ZEBRA_H
#define __ZEBRA_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "public.h"

using namespace cv;

void Zabra_Init(void);
void Zebra_Detect(Mat *disp,Mat *img_in);

#endif
