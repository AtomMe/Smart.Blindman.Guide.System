#ifndef __UARTCMD_H
#define __UARTCMD_H

#include "public.h"

extern char CMD_TX_Point[4];//point axis
extern char CMD_TX_Text[4];//sound 

void UART_Init(void);
void Close_UART(void);
int uart_com(char *cmd,char *data);//
void mpu6050_data_get(short *a);//pitch roll yaw

#endif
