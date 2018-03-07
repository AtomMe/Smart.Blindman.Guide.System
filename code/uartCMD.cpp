#include<stdio.h>
#include <iostream>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>

char CMD_TX_Point[4]={0x8a,0x8b,0x1c,0xae};
char CMD_TX_Text[4]={0x8a,0x8b,0x1c,0xad};
int pfile;

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
	newtio.c_cflag |=  CSTOPB;
	newtio.c_cc[VTIME]  = 0;//重要
	newtio.c_cc[VMIN] = 100;//返回的最小值  重要
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	printf("uart set done!\n\r");
	return 0;
}
int uart_init(int baund)//p为标志文件的变量指针
{
int nset1;
pfile = open("/dev/ttyUSB0", O_RDWR|O_NONBLOCK);//打开串口  |O_NONBLOCK
if (pfile == -1)
{  
	printf("uart open error!\n");    
	exit(1);  
}
nset1 = set_opt(pfile,115200, 8, 'N', 1);//设置串口属性  
if (nset1 == -1)
{
    	printf("uart set error!\n");  
    	exit(1);  
}
return 1;	
}
int uart_com(char *cmd,char *data)//data最长200字节，第一个字节表示data的长度，数据的最后一个字节为异或校验位，长度不包括校验位,fd为文件标志
{
int i,nwrite,len=strlen(data)+2;
char uart_tx_buf[204];
memset(uart_tx_buf,0,204);
uart_tx_buf[0]=*(cmd);
uart_tx_buf[1]=*(cmd+1);
uart_tx_buf[2]=*(cmd+2);
uart_tx_buf[3]=*(cmd+3);
uart_tx_buf[4]=len;
for(i=0;i<len;i++)
{
	uart_tx_buf[i+5]=*(data+i);
	uart_tx_buf[len+4]=uart_tx_buf[len+4]^*(data+i);	
}
nwrite=write(pfile,uart_tx_buf,len+5);
if(nwrite>0)
{
	//printf("uart write suscess%dBytes!\n",nwrite);
	return nwrite;
}
}

void mpu6050_data_get(short *a)
{
	int nread,i;
	unsigned char xorJiaoYan=0,b[10];
	nread=read(pfile,b,10);
	if(nread>0)
	{
		//printf("uart read suscess%dBytes!\n",nread);
	}
/*	for(i=0;i<10;i++)
	{
		printf("uart uart b=%x ",*(b+i));	
	}*/
	for(i=0;i<9;i++)
	{
		xorJiaoYan^=b[i];	
	}
	if(b[0]==0x88&&b[1]==0xa1&&b[2]==0x06&&xorJiaoYan==b[9])
	{
		//printf("uart data update!\n");
		*a=(short)(b[3]<<8)+b[4];
		*(a+1)=(short)(b[5]<<8)+b[6];
		*(a+2)=(short)(b[7]<<8)+b[8];
	}
	
}
void UART_Init(void)
{
	uart_init(11520);
}
void Close_UART(void)
{
	close(pfile);  
}



