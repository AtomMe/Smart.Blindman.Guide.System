#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include <cstdio>
#include <iostream>
#include "zebra.h"

using namespace std;
using namespace cv;

String face_cascade_name = "403012.xml";
CascadeClassifier face_cascade;

void Zabra_Init(void)//IO
{
	face_cascade.load(face_cascade_name);
}
void Zebra_Detect(Mat *disp,Mat *img_in)
{
	std::vector<Rect> faces;
	Mat in;
	equalizeHist( *img_in, *img_in );
	resize(*img_in,in,Size(60,45),0,0,CV_INTER_LINEAR);
	//-- Detect faces
	face_cascade.detectMultiScale( in, faces, 1.2, 0,0);
	if(faces.size()>0)
	{
		putText(*disp, "Zabra:exist",
			Point(20, 40),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(255, 255,255),
			3, LINE_AA); 
		putText(*disp, "Zabra:exist",
			Point(20, 40),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(0, 255,0 ),
			1, LINE_AA); 
	    Comm_SendVoice((char *)"°ßÂíÏß");
	}
	else
	{
		putText(*disp, "Zabra:lost",
			Point(20, 40),
			FONT_HERSHEY_COMPLEX, 1,
			Scalar(0, 0, 255),
			1, LINE_AA);
	}
}
