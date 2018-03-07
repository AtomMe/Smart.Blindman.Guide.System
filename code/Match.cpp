#include <math.h>
#include <opencv2/core.hpp>
#include <cstdio>
#include "Match.h"


using namespace cv;

//常量定义
const double om[3][3]={
	0.999858849394024,		0.00183479936472499,	0.0167007424909780,
	-0.00135572667381734,	0.999588528805766,		-0.0286519647687495,
	-0.0167464412232765,	0.0286252788844924,		0.999449923765639
};
const double T[3]={ -316.400996750417,
					-3.75530395287669,
					-14.3327631459247};
Cam_Param left={
	706.384936188235,704.893813617299,
	314.259223441088,233.135665417066,
	-0.134546690713906,-0.0293652382985484,-0.00483589765737971,-0.00550020496467129
};
Cam_Param right={
	707.854959612545,707.483244370773,
	338.401231204644,237.133160186017,
	-0.131703572521893,-0.224353081550557,-0.00410080879713886,-0.000424743177265923
};
//运行数组,数据定义
float Cal_Img1[Img_X][Img_Y][2];	//校准数据
float Cal_Img2[Img_X][Img_Y][2];

Img_Axis *Xp_To_Xn(Img_Axis *p,Cam_Param *c)
{
	float xp=p->xn,yp=p->yn;
	float xd=(xp-c->cc[0])/c->fc[0];
	float yd=(yp-c->cc[1])/c->fc[1];
	float xn=xd,yn=yd,r2,kr,dx,dy;
	for(int i=0;i<20;i++)
	{
		r2=xn*xn+yn*yn;
		kr=1+c->kc[0]*r2+c->kc[1]*r2*r2;
		dx=2*c->kc[2]*xn*yn+c->kc[3]*(r2+2*xn*xn);
		dy=c->kc[2]*(r2+2*yn*yn)+2*c->kc[3]*xn*yn;
		xn=(xd-dx)/kr;
		yn=(yd-dy)/kr;
	}
	p->xn=xn;
	p->yn=yn;
	return p;
}
void ImgCalDat_Init(void)
{
	Img_Axis *input=new Img_Axis;
	Img_Axis *out1,*out2;

	Cam_Param *param1=&left,*param2=&right;
	for(int i=0;i<Img_X;i++)
	{
		for(int j=0;j<Img_Y;j++)
		{
#if(Img_X==480 && Img_Y==640)
			input->xn=i;
			input->yn=j;
			out1=Xp_To_Xn(input,param1);
			Cal_Img1[i][j][0]=out1->xn;
			Cal_Img1[i][j][1]=out1->yn;
			input->xn=i;
			input->yn=j;
			out2=Xp_To_Xn(input,param2);
			Cal_Img2[i][j][0]=out2->xn;
			Cal_Img2[i][j][1]=out2->yn;
#else
	#if(Img_X==240 && Img_Y==320)
			float tmp[2][4];
			for(int cnt=0;cnt<4;cnt++)
			{
				input->xn=i*2+cnt&0x01;
				input->yn=j*2+(cnt>>1);
				out1=Xp_To_Xn(input,param1);
				tmp[0][cnt]=out1->xn;
				tmp[1][cnt]=out1->yn;
			}
			Cal_Img1[i][j][0]=(tmp[0][0]+tmp[0][1]+tmp[0][2]+tmp[0][3])/4;
			Cal_Img1[i][j][1]=(tmp[1][0]+tmp[1][1]+tmp[1][2]+tmp[1][3])/4;
			//
			for(int cnt=0;cnt<4;cnt++)
			{
				input->xn=i*2+cnt&0x01;
				input->yn=j*2+(cnt>>1);
				out1=Xp_To_Xn(input,param2);
				tmp[0][cnt]=out1->xn;
				tmp[1][cnt]=out1->yn;
			}
			Cal_Img2[i][j][0]=(tmp[0][0]+tmp[0][1]+tmp[0][2]+tmp[0][3])/4;
			Cal_Img2[i][j][1]=(tmp[1][0]+tmp[1][1]+tmp[1][2]+tmp[1][3])/4;
	#endif
#endif
			
		}

	}	
	//input->xn=182;input->yn=89;
	//param1=&left;
	//output=Xp_To_Xn(input,param1);
	//cout<<"x1="<<output->xn<<','<<output->yn<<endl;
}

void Cal3DPixels(int xi1,int yi1,int xi2,int yi2,float *Axis)
{
	float x1=Cal_Img1[xi1][yi1][0];
	float y1=Cal_Img1[xi1][yi1][1];
	float x2=Cal_Img2[xi2][yi2][0];
	float y2=Cal_Img2[xi2][yi2][1];

	float k1=om[0][0]*x1+om[0][1]*y1+om[0][2];
	float k2=om[1][0]*x1+om[1][1]*y1+om[1][2];
	float k3=om[2][0]*x1+om[2][1]*y1+om[2][2];
	
	Mat Z=Mat::zeros(2,2,CV_32FC1);
	float *p1=Z.ptr<float>(0),*p2=Z.ptr<float>(1);
	p1[0]=x2*x2+y2*y2+1;
	p1[1]=-k1*x2-k2*y2-k3;
	p2[0]=p1[1];
	p2[1]=k1*k1+k2*k2+k3*k3;
	Z=Z.inv();

	float k[2][2];
	k[0][0]=p1[0];k[0][1]=p1[1];
	k[1][0]=p2[0];k[1][0]=p2[1];
	
	float zc1=(k[0][0]*x2-k[0][1]*k1)*T[0]+(k[0][0]*y2-k[0][1]*k2)*T[1]+(k[0][0]-k[0][1]*k3)*T[2];
	float zc2=(k[1][0]*x2-k[1][1]*k1)*T[0]+(k[1][0]*y2-k[1][1]*k2)*T[1]+(k[1][0]-k[1][1]*k3)*T[2];
//保存数据
#define limit(k) (k<dmin? 0:k>dmax? 0:k)
	zc1=limit(zc1);
	if(zc1)
	{
		Axis[0]=xi1;
		Axis[1]=yi1;
		Axis[2]=zc1;
	}
	else
	{
		Axis[0]=0;
		Axis[1]=0;
		Axis[2]=0;
	}
}

