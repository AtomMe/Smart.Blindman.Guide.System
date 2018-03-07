#ifndef FREEGLUTAPP_H
#define FREEGLUTAPP_H
#include "public.h"

#define Max_Item 12

void Disp_Init(int argc, char *argv[]);
void Disp_Next(float displaydata[12][12]);
void Disp_End(void);

#endif
