#include <math.h>
#include <GL/freeglut.h> 
#include "opencv2/core.hpp"
#include "freeglutapp.h"
#include "public.h"
#include "MatchPoint.h"

#define pi 3.1415926f
#define thes1 (2*pi/3)
#define thes2 (4*pi/3)
#define ver_len 0.25f

#define normal(dat) (dat>1? 1:dat<0? 0:dat)
#define limrange(k) (k>drange? drange:k<0? 0:k)
using namespace std;
using namespace cv;

float img[12][12];
float scalar=50;
bool mouseisdown=false;  
bool loopr=false;  
int mx,my;  
int ry=1;  
int rx=1; 

float vertex_base[][3] = { //长0.5 宽0.5 高0.5 以原点为对称中心
	0,			0,			0, 
	ver_len,	0,			0, 
	0,			ver_len,	0, 
	ver_len,	ver_len,	0, 
	0,			0,			ver_len, 
	ver_len,	0,			ver_len, 
	0,			ver_len,	ver_len, 
	ver_len,	ver_len,	ver_len, 
};
static GLint index_list[][4] = {
    0, 2, 3, 1,
    0, 4, 6, 2,
    0, 1, 5, 4,
    4, 5, 7, 6,
    1, 3, 7, 5,
    2, 6, 7, 3,
};


void timer(int p)  
{  
	ry-=5;  
	//marks the current window as needing to be redisplayed.  
	glutPostRedisplay();  
	if (loopr)  
		glutTimerFunc(200,timer,0);  
}  
  
  
void mouse(int button, int state, int x, int y)  
{  
	if(button == GLUT_LEFT_BUTTON)  
	{  
		if(state == GLUT_DOWN)  
		{  
			mouseisdown=true;  
			loopr=false;  
		}  
		else  
		{  
			mouseisdown=false;  
		}  
	}  
  
	if (button== GLUT_RIGHT_BUTTON)  
	if(state == GLUT_DOWN)  
	{  
		loopr=true; 
		glutTimerFunc(200,timer,0);  
	}  

}  
  
void motion(int x, int y)  
{  
    if(mouseisdown==true)  
    {  
       ry+=x-mx;  
        rx+=y-my;  
        mx=x;  
        my=y;  
        glutPostRedisplay();  
    }  

}  

void CalRGB(float h,float *r,float *g,float *b)
{
	h-=(int)h;
	if(h<0.1) 
	{
		*r=1;
		*g=1;
		*b=1;
		return ;
	}
	h*=3/2*pi;
	if(h<thes1)
	{
		*b=0;
		*r=float((1+cos(h))/(cos(pi/3-h)));
		*g=3-*r-*b;
	}
	else if(h<thes2)
	{
		*r=0;
		*g=float((1+cos(h-thes1))/(cos(pi-h)));
		*b=3-*r-*g;
	}
	else
	{
		*g=0;
		*b=float((1+cos(h-thes2))/(cos(5*pi/3-h)));
		*r=3-*g-*b;
	}
	*r=normal(*r);
	*g=normal(*g);
	*b=normal(*b);
}
void DrawCube(int x,int y,float h)
{
	float vertex_list[8][3];
	float r,g,b;
	for(int i=0;i<8;i++)
	{
		vertex_list[i][0]=vertex_base[i][0]+(float)y*ver_len;
		vertex_list[i][1]=vertex_base[i][1]+(float)x*ver_len;
		if(i>3)//(i>>1)&0x01) 
			vertex_list[i][2]=-h;
		else
			vertex_list[i][2]=0;
	}
	CalRGB(h,&r,&g,&b);
	glColor3f(r,g,b);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT,0, vertex_list);
	//glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, index_list);
	glDrawElements(GL_LINE_LOOP, 24, GL_UNSIGNED_INT, index_list);
}
void display(void) 
{    
	glClear (GL_COLOR_BUFFER_BIT);  
	glLoadIdentity();//将中心点（0.0,0.0）移到窗口中心
	gluLookAt (0.0, 0.0, 7.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0); 
	//一组数据就是脑袋的位置,第二组数据就是眼睛看的物体的位置,第三组就是头顶朝向的方向(因为你可以歪着头看同一个物体) 

	glRotatef(ry,0,1,0);  //旋转角度+坐标（x,y,z)
	glRotatef(rx-180,1,0,0);  
  
	glPointSize(1.0);  
	if(disp_change==false)
	{
		for(int i=0;i<Max_Item;i++)
		{
			for(int j=0;j<Max_Item;j++)
			{
				DrawCube(i-Max_Item/2,j-Max_Item/2,3*img[i][j]);
				img[i][j]=0;
			}
		}
	}
	else
	{
		glBegin(GL_POINTS);
			glColor3f(1,1,1);
			glVertex3f(0,0,0);
			float x,y,z,r,g,b;
			for(int i=0;i<nPoints;i++)
			{
				x=(Mpoint[i][0]-Img_X/2)/Img_X;
				y=(Mpoint[i][1]-Img_Y/2)/Img_Y;
				z=Mpoint[i][2]/drange;
				if(z!=0)
				{
					CalRGB(z,&r,&g,&b);
					glColor3f(r,g,b);
					glVertex3f(y*8,x*6,z*5); 	
				}
			}
		glEnd();
	}
	//坐标系
#define base_x -3
#define base_y -3
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);//R,G,B x
		glVertex3f(base_x,base_y,0.0);
		glVertex3f(base_x+1,base_y,0.0);
		glColor3f(0.0,1.0,0.0);// y
		glVertex3f(base_x,base_y,0.0);
		glVertex3f(base_x,base_y+1,0.0);
		glColor3f(0.0,0.0,1.0);// z
		glVertex3f(base_x,base_y,0.0);
		glVertex3f(base_x,base_y,1.0);
	glEnd();
	glFlush();  
 }  
 
void reshape (int w, int h) {  
   glViewport (0, 0, (GLsizei)w, (GLsizei)h);//set viewpoint  
   glMatrixMode (GL_PROJECTION);//specify which matrix is the current matrix  
   glLoadIdentity ();//replace the current matrix with the identity matrix  
   gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);  
   glMatrixMode (GL_MODELVIEW);  
}
void special(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        ry+=5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
       ry-=5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP:
        rx-=5;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        rx+=5;
        glutPostRedisplay();
        break;
	case GLUT_KEY_END:
		run_flag=false;
    }
}
void Disp_Init(int argc, char *argv[])
{
	glutInit(&argc, argv);//initialize the GLUT library  
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);//sets the initial display mode  
	glutInitWindowPosition(100,100);  
	glutInitWindowSize(480,320);  
	glutCreateWindow("->3D Test<-");  
	glutDisplayFunc(display);  
	glutReshapeFunc (reshape);  
	glutMouseFunc(mouse);  
	glutMotionFunc(motion);
	glutSpecialFunc(special);
}
void Disp_Next(float displaydata[12][12])
{
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<12;j++)
		{
			float k=limrange(displaydata[i][j]);
			if(k!=0)
			{
				if(show_distance==false)
				{
					img[i][j]=1-k/drange;
				}
				else
				{
					img[i][j]=k/drange;
				}
			}
		}
	}
	glutPostRedisplay();   
	glutMainLoopEvent();
}
void Disp_End(void)
{
	glutMainLoop();
}
