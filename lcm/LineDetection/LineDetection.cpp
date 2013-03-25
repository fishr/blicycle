/*
 * LineDetection.cpp
 *
 *  Created on: Mar 9, 2013
 *      Author: blicycle
 */




/**
 * @file HoughLines_Demo.cpp
 * @brief Demo code for Hough Transform
 * @author OpenCV team
 */

#include "opencv/cv.h"
#include "opencv/cvaux.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/// Global variables

/** General variables */
Mat src, edges;
Mat src_gray;
Mat blurred;
Mat standard_hough, probabilistic_hough;
int min_threshold = 50;
int max_trackbar = 500;

const char* standard_name = "Standard Hough Lines Demo";
const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int s_trackbar = max_trackbar;
int p_trackbar = max_trackbar;

/// Function Headers
void help();
void Standard_Hough( int, void* );
void Probabilistic_Hough( int, void* );

/**
 * @function main
 */
int main( int, char** argv )
{
   /// Read the image
   src = imread( "/home/blicycle/Desktop/blicycle/lcm/LineDetection/Debug/track_curved_hash.jpg", 1 );

   if( src.empty() )
     { help();
       return -1;
     }

   src.adjustROI(-300, 0, 0, 0 );

   ///blur for comedic effect
   blur( src, blurred, Size( 3, 5 ), Point(-1,-1) );
   namedWindow("blurred", CV_WINDOW_AUTOSIZE);
   imshow("blurred", blurred);

   /// Pass the image to gray
   cvtColor( blurred, src_gray, CV_RGB2GRAY );

   /// Apply Canny edge detector
   //Canny( src_gray, edges, 100, 300, 3 );

   ///try filtering out the white since we have lines anyway
   threshold(src_gray, edges, 100, 255, THRESH_BINARY );
   namedWindow("gray", CV_WINDOW_AUTOSIZE);
   imshow("gray", edges);

   /// Create Trackbars for Thresholds
   char thresh_label[50];
   sprintf( thresh_label, "Thres: %d + input", min_threshold );

   //namedWindow( standard_name, CV_WINDOW_AUTOSIZE );
   //createTrackbar( thresh_label, standard_name, &s_trackbar, max_trackbar, Standard_Hough);

   namedWindow( probabilistic_name, CV_WINDOW_AUTOSIZE );
   createTrackbar( thresh_label, probabilistic_name, &p_trackbar, max_trackbar, Probabilistic_Hough);

   /// Initialize
   //Standard_Hough(0, 0); //it detects too many lines, the other one places preference
   	   	   	   	   	   	   //for more lines with there already are some, good!!
   Probabilistic_Hough(0, 0);
   waitKey(0);
   return 0;
}

/**
 * @function help
 * @brief Indications of how to run this program and why is it for
 */
void help()
{
  printf("\t Hough Transform to detect lines \n ");
  printf("\t---------------------------------\n ");
  printf(" Usage: ./HoughLines_Demo <image_name> \n");
}

int sort_lower_x(const void *a, const void *b)
{
	Vec<int,4> *Va = (Vec<int, 4> *) a;
	Vec<int,4> *Vb = (Vec<int, 4> *) b;
	//printf("%i \n", Va->val[0]);
	if ((Va->val[0])>(Vb->val[0])){
		return 1;
	}else{
		return -1;
	}
}

/**
 * @function Standard_Hough
 */
void Standard_Hough( int, void* )
{
  vector<Vec2f> s_lines;
  cvtColor( edges, standard_hough, CV_GRAY2BGR );

  /// 1. Use Standard Hough Transform
  HoughLines( edges, s_lines, 1, CV_PI/180, min_threshold + s_trackbar, 0, 0 );

  /// Show the result
  for( size_t i = 0; i < s_lines.size(); i++ )
     {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r*cos_t, y0 = r*sin_t;
      double alpha = 1000;

       Point pt1( cvRound(x0 + alpha*(-sin_t)), cvRound(y0 + alpha*cos_t) );
       Point pt2( cvRound(x0 - alpha*(-sin_t)), cvRound(y0 - alpha*cos_t) );
       line( standard_hough, pt1, pt2, Scalar(255,0,0), 3, CV_AA);
     }

   imshow( standard_name, standard_hough );
}

/**
 * @function Probabilistic_Hough
 */
void Probabilistic_Hough( int, void* )
{
  vector<Vec4i> p_lines;
  cvtColor( edges, probabilistic_hough, CV_GRAY2BGR );

  /// 2. Use Probabilistic Hough Transform
  HoughLinesP( edges, p_lines, 1, CV_PI/180, min_threshold + p_trackbar, 30, 200);
  //I patched the probability based Hough Lines in opencv/modules/imgproc/hough.cpp
  //so that the output vectors will always have the lesser y value as their first y value
  /**************************************************************************************
   * if( good_line )
        {
	CvRect lr;
	    if(line_end[0].y<line_end[1].y){
        	CvRect lr = { line_end[0].x, line_end[0].y, line_end[1].x, line_end[1].y };
	    }else{
		CvRect lr = { line_end[1].x, line_end[1].y, line_end[0].x, line_end[0].y };
	    }
            cvSeqPush( lines, &lr );
            if( lines->total >= linesMax )
                return;
        }
   **************************************************************************************/


  //Sort by the lower X value to sort into similar line beginnings
  qsort(p_lines.data(), p_lines.size(), sizeof(Vec4i), sort_lower_x);

  vector<Vec4i> lowerXs(p_lines.size()/2);
  lowerXs[0] = p_lines[0];

  for( size_t i = 1; i<p_lines.size()/2; i++ )
     {
	  int offset=0;
	  if (abs(lowerXs[i-1-offset][0]-p_lines[i*2][0])>70)
	  {
		  lowerXs[i-offset] = p_lines[i];
	  }else{
		  if (abs(lowerXs[i-1-offset][2]-p_lines[i*2][2])>70){
			  lowerXs[i-offset] = p_lines[i*2];
		  }else{
			  offset++;

			  lowerXs[i-offset][0] = (lowerXs[i-1][0]+p_lines[i*2][0])/2;
			  lowerXs[i-offset][1] = min(lowerXs[i-1][1], p_lines[i*2][1]);
			  lowerXs[i-offset][2] = (lowerXs[i-1][2]+p_lines[i*2][2])/2;
			  lowerXs[i-offset][3] = max(lowerXs[i-1][3], p_lines[i*2][3]);

		  }
	  }

     }
  Vec4i testing;
  testing[3]=0;
  lowerXs.push_back(testing);

  while(lowerXs.back()[3]==0){
	  lowerXs.pop_back();
	  printf("%i", (int) lowerXs.size());
  }

  qsort(lowerXs.data(), lowerXs.size(), sizeof(Vec4i), sort_lower_x);

  vector<Vec4i> reducedLines(lowerXs.size());
  reducedLines[0] = lowerXs[0];

  for( size_t i = 1; i<lowerXs.size(); i++ )
     {
	  if (abs(reducedLines[i-1][0]-lowerXs[i][0])>50)
	  {
		  reducedLines[i] = lowerXs[i];
	  }else{
		  if (abs(reducedLines[i-1][2]-lowerXs[i][2])>50){
			  reducedLines[i] = lowerXs[i];
		  }else{
			  reducedLines[i-1][0] = 0;
			  reducedLines[i-1][1] = 0;
			  reducedLines[i-1][2] = 0;
			  reducedLines[i-1][3] = 0;

			  reducedLines[i][0] = (reducedLines[i-1][0]+lowerXs[i][0])/2;
			  reducedLines[i][1] = min(reducedLines[i-1][1], lowerXs[i][1]);
			  reducedLines[i][2] = (reducedLines[i-1][2]+lowerXs[i][2])/2;
			  reducedLines[i][3] = max(reducedLines[i-1][3], lowerXs[i][3]);

		  }
	  }

     }


  /// Show the result
  for( size_t i = 0; i < reducedLines.size(); i++)//p_lines.size(); i++ )
     {
       Vec4i l = reducedLines[i];//p_lines[i];
       line( probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
     }

   imshow( probabilistic_name, probabilistic_hough );
}
