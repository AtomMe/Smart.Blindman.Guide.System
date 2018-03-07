#ifndef MATCH_H
#define MATCH_H
#include "public.h"
//结构体定义
typedef struct{
	float xn;
	float yn;
}Img_Axis;
typedef	struct {
	float fc[2];
	float cc[2];
	float kc[4];
}Cam_Param;

extern const double om[3][3];
extern const double T[3];
extern Cam_Param left;
extern Cam_Param right;
extern float Cal_Img1[Img_X][Img_Y][2];
extern float Cal_Img2[Img_X][Img_Y][2];

void ImgCalDat_Init(void);
Img_Axis *Xp_To_Xn(Img_Axis *p,Cam_Param *c);
void Cal3DPixels(int xi1,int yi1,int xi2,int yi2,float *Axis);

#endif
