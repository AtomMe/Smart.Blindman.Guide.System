#ifndef __COMMUNICATE_H
#define __COMMUNICATE_H
#include "public.h"

void Comm_Init(void);
void Comm_GetAngle(short *angle);
void Comm_SendVoice(char *v);
void Comm_Send_144(float dat[12][12]);
void Comm_Close(void);

#endif
