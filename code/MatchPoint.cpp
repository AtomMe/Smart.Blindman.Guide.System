#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "Match.h"
#include "trafficlight.h"
#include "zebra.h"

using namespace std;
using namespace cv;

int nPoints;
float Mpoint[Img_X*Img_Y][3];
float kp0[Img_X*Img_Y][2],kp1[Img_X*Img_Y][2];
vector<String> typeDesc;
vector<String> typeAlgoMatch;
	
void Match_Init(void)
{

    //typeDesc.push_back("AKAZE-DESCRIPTOR_KAZE_UPRIGHT");   
    //typeDesc.push_back("AKAZE");    
    typeDesc.push_back("ORB");       
    typeAlgoMatch.push_back("BruteForce");
    typeAlgoMatch.push_back("BruteForce-L1");
    typeAlgoMatch.push_back("BruteForce-Hamming");
	typeAlgoMatch.push_back("BruteForce-Hamming(2)");
	ImgCalDat_Init();//Data Init
}
void Match_Point_Detect(Mat *img1,Mat *img2,Mat *out,float imgdata[12][12])
{		
		nPoints=0;
		try{
			vector<String>::iterator itDesc;
			for (itDesc = typeDesc.begin(); itDesc != typeDesc.end(); itDesc++)
			{
				Ptr<Feature2D> b;
				Ptr<DescriptorMatcher> descriptorMatcher;
				vector<DMatch> matches;
				vector<KeyPoint> keyImg1, keyImg2;
				Mat descImg1, descImg2;
				vector<String>::iterator itMatcher = typeAlgoMatch.end();
				if (*itDesc == "AKAZE-DESCRIPTOR_KAZE_UPRIGHT"){
					b = AKAZE::create(AKAZE::DESCRIPTOR_KAZE_UPRIGHT);
					}
				else if (*itDesc == "AKAZE"){
					b = AKAZE::create();
					}
				else if (*itDesc == "ORB"){
					b = ORB::create();
				}
				b->detectAndCompute(*img1, Mat(),keyImg1, descImg1,false);
				b->detectAndCompute(*img2, Mat(),keyImg2, descImg2,false);

				for (itMatcher = typeAlgoMatch.begin(); itMatcher != typeAlgoMatch.end(); itMatcher++)
				{
					descriptorMatcher = DescriptorMatcher::create(*itMatcher);
					if ((*itMatcher == "BruteForce-Hamming" || *itMatcher == "BruteForce-Hamming(2)") && (b->descriptorType() == CV_32F || b->defaultNorm() <= NORM_L2SQR))
					{
						continue;
					}
					if ((*itMatcher == "BruteForce" || *itMatcher == "BruteForce-L1") && (b->defaultNorm() >= NORM_HAMMING))
					{
						continue;
					}
					try
					{
						descriptorMatcher->match(descImg1, descImg2, matches, Mat());//—∞’“∆•≈‰µ„
						int nbMatch=int(matches.size());
						
						for(int i=0;i<nbMatch;i++)
						{
							kp0[nPoints][0]=keyImg1[matches[i].queryIdx].pt.x;
							kp0[nPoints][1]=keyImg1[matches[i].queryIdx].pt.y;
							kp1[nPoints][0]=keyImg2[matches[i].trainIdx].pt.x;
							kp1[nPoints][1]=keyImg2[matches[i].trainIdx].pt.y;

							nPoints++;
							if(nPoints>Img_X*Img_Y-1) nPoints=0;
						}
					}
					catch (Exception& e){cout<<e.msg<<endl;}
				}
			}
		}
		catch (Exception& e){cout<<e.msg<<endl;}
		//
		Mat point1(nPoints,2,CV_32F);
		Mat point2(nPoints,2,CV_32F);
		for(int i=0;i<nPoints;i++)
		{
			point1.at<float>(i,0)=kp0[i][0];
			point1.at<float>(i,1)=kp0[i][1];
			point2.at<float>(i,0)=kp1[i][0];
			point2.at<float>(i,1)=kp1[i][1];
		}
		
	#if SHOW_MATCH_OUT
		vector<DMatch> 	bestMatches,match;
		vector<Point2f> keyImg1, keyImg2;
		DMatch matchdat;
		Point2f keydat;
	#endif
		vector<uchar> 	m_RANSACStatus;
		findFundamentalMat(point1, point2, m_RANSACStatus);
		int OutPointCount = 0,real_cnt=0;
		int x1,x2,y1,y2;
		if(m_RANSACStatus.size()>0)
		{
			for (int i=0; i<nPoints; i++)
			{		
				x1=kp0[i][0];
				y1=kp0[i][1];
				x2=kp1[i][0];
				y2=kp1[i][1];
			#if SHOW_MATCH_OUT
				keydat.x=x1;
				keydat.y=y1;
				keyImg1.push_back(keydat);
				keydat.x=x2;
				keydat.y=y2;
				keyImg2.push_back(keydat);
				matchdat.queryIdx=OutPointCount;
				matchdat.trainIdx=OutPointCount;
				match.push_back(matchdat);
			#endif
				if (m_RANSACStatus[i] != 0)
				{
					
					int err=y1-y2;
					if(err<0) err=-err;
					if(err<40)
					{
					#if SHOW_MATCH_OUT
						bestMatches.push_back(matchdat);
					#endif
						Cal3DPixels(y1,x1,y2,x2,Mpoint[OutPointCount]);
						real_cnt++;
					}
				}
				OutPointCount++;
			}
		}
		else
		{
			for (int i=0; i<nPoints; i++)
			{
				x1=kp0[i][0];
				y1=kp0[i][1];
				x2=kp1[i][0];
				y2=kp1[i][1];
			#if SHOW_MATCH_OUT
				keydat.x=x1;
				keydat.y=y1;
				keyImg1.push_back(keydat);
				keydat.x=x2;
				keydat.y=y2;
				keyImg2.push_back(keydat);
				matchdat.queryIdx=OutPointCount;
				matchdat.trainIdx=OutPointCount;
				match.push_back(matchdat);
			#endif
				int err=y1-y2;
				if(err<0) err=-err;
				if(err<40)
				{
				#if SHOW_MATCH_OUT
					bestMatches.push_back(matchdat);
				#endif
					Cal3DPixels(y1,x1,y2,x2,Mpoint[OutPointCount]);
					real_cnt++;
				}
				OutPointCount++;
			}
		}
	#if SHOW_MATCH_OUT
		vector<KeyPoint> key1(OutPointCount);
		vector<KeyPoint> key2(OutPointCount);
		KeyPoint::convert(keyImg1, key1);
		KeyPoint::convert(keyImg2, key2);
		show_match=false;
		drawMatches(*img1, key1, *img2, key2,bestMatches,*out);
		show_match=true;
	#endif
		int err=nPoints-real_cnt;
		nPoints=real_cnt;

		int imgcnt[Img_X][Img_Y];
		memset(imgcnt,0,Img_X*Img_Y*sizeof(int));
		memset(imgdata,0,12*12*sizeof(float));
		for(int i=0;i<real_cnt;i++)
		{
			x1=Mpoint[i][0]/20;
			y1=Mpoint[i][1]/27;
			float high=Mpoint[i][2];
			if(imgcnt[x1][y1])
			{
				imgcnt[x1][y1]++;
				imgdata[x1][y1]=high/imgcnt[x1][y1]+imgdata[x1][y1]*(imgcnt[x1][y1]-1)/imgcnt[x1][y1];
			}
			else
			{
				imgcnt[x1][y1]=1;
				imgdata[x1][y1]=high;
			}
		}	
}

