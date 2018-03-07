#ifndef __PUBLIC_H
#define __PUBLIC_H
#define HAVE_TBB
#include <iostream>
#include <cstdio>
#include "communicate.h"

extern bool run_flag;
extern bool show_distance;
extern bool show_match;
extern bool disp_change;

#define USE_UART 1
#define SHOW_MATCH_OUT 1

#define Img_X 240
#define Img_Y 320
#define dmax  40000
#define dmin -40000
#define drange 10000

#endif
