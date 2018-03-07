#include "communicate.h"
#include "uartCMD.h"

#define lim(k) (k>140 ? 140:k<1 ? 1:k)

#if USE_UART
void Comm_Init(void)
{
	UART_Init();
}
void Comm_GetAngle(short *angle)
{
	mpu6050_data_get(angle);
}
void Comm_SendVoice(char *v)
{
	uart_com(CMD_TX_Text,v);
}
void PutL2R_U2D(float dat[12][12],int startX,int startY,unsigned char *out)
{
	int cnt=0;
	for(int i=startX;i<startX+6;i++)
	{
		for(int j=startY;j<startY+6;j++)
		{
			out[cnt]=141-lim(dat[i][j]/drange*140);
			cnt++;
		}
	}
}
void PutU2D_R2L(float dat[12][12],int startX,int startY,unsigned char *out)
{
	int cnt=0;
	for(int j=startY;j>startY-6;j--)
	{
		for(int i=startX;i<startX+6;i++)
		{
			out[cnt]=141-lim(dat[i][j]/drange*140);
			cnt++;
		}
	}
}
void PutR2L_D2U(float dat[12][12],int startX,int startY,unsigned char *out)
{
	int cnt=0;
	for(int i=startX;i>startX-6;i--)
	{
		for(int j=startY;j>startY-6;j--)
		{
			out[cnt]=141-lim(dat[i][j]/drange*140);
			cnt++;
		}
	}
}
void PutD2U_L2R(float dat[12][12],int startX,int startY,unsigned char *out)
{
	int cnt=0;
	for(int j=startY;j<startY+6;j++)
	{
		for(int i=startX;i>startX-6;i--)
		{
			out[cnt]=141-lim(dat[i][j]/drange*140);
			cnt++;
		}
	}
}
void Comm_Send_144(float dat[12][12])
{
	unsigned char out[150],*p;
	PutL2R_U2D(dat,0,0,out);
	out[36]=out[0];
	PutU2D_R2L(dat,0,11,out+37);
	out[73]=out[37];
	PutR2L_D2U(dat,11,11,out+74);
	out[110]=out[74];
	PutD2U_L2R(dat,11,0,out+111);
	out[147]=out[111];
	out[148]=0;
	/*
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<12;j++)
		{
			printf("%3d",(int)dat[i][j]);
		}
		putchar('\n');
	}
	int cnt=0;
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<12;j++)
		{
			printf("%3d",out[cnt]);
			if(j==5) putchar('\n');
			cnt++;
		}
		if((i+1)%3==0) 
		{
			printf("\ndat=%d\n",out[cnt]);
			cnt++;
		}
		putchar('\n');
	}
	*/
	uart_com(CMD_TX_Point,(char*)out);
}
void Comm_Close(void)
{
	Close_UART();
}
#else
void Comm_Init(void){};
void Comm_GetAngle(int *angle){};
void Comm_SendVoice(char *v){};
void Comm_Send_144(float dat[12][12]){};
void Comm_Close(void){};
#endif


