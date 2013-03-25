//============================================================================
// Name        : BlobLanes.cpp
// Author      : Ryan Fish
// Version     :
// Copyright   : MIT License
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "opencv/cv.h"
#include "opencv/cvaux.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
using namespace std;
using namespace cv;

Mat src, blurred, src_gray, edges, element, denoised_img, lanes;

int min_threshold = 50;
int max_trackbar = 200;
int blacktowhite = 0;

const char* standard_name = "Standard Hough Lines Demo";
const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int p_trackbar = 116;

int smoothfactor=20;

int xup =16;
int yup =0;
int xdown =3;
int ydown =3;


int expectedLaneWidth = 40;

void blacknwhite(int, void*);
void denoise(int, void*);
void findLanes();

int main() {

	/// Read the image
	src = imread( "/home/blicycle/Desktop/blicycle/lcm/LineDetection/Debug/track_curved_hash.jpg", 1 );

	src.adjustROI(-300, 0, 0, 0 );


	/// Pass the image to gray
	cvtColor( src, src_gray, CV_RGB2GRAY );

	///blur for comedic effect
	//blur( src, blurred, Size( 3, 5 ), Point(-1,-1) );
	pyrDown(src_gray, blurred);
	namedWindow("blurred", CV_WINDOW_AUTOSIZE);
	imshow("blurred", blurred);


	/// Apply Canny edge detector
	//Canny( src_gray, edges, 100, 300, 3 );
	threshold(blurred, edges, p_trackbar, 255, THRESH_BINARY );
	namedWindow("gray", CV_WINDOW_AUTOSIZE);
	imshow("gray", edges);

	element = getStructuringElement(MORPH_RECT, Size(xup+1, yup+1), Point(-1,-1));
	dilate(edges, denoised_img, element);
	element = getStructuringElement(MORPH_RECT, Size(xdown+1, ydown+1), Point(-1,-1));
	erode(denoised_img, denoised_img, element);
	namedWindow("denoise", CV_WINDOW_AUTOSIZE);
	imshow("denoise", denoised_img);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf( thresh_label, "Thres: %d + input", min_threshold );

	//namedWindow( standard_name, CV_WINDOW_AUTOSIZE );
	//createTrackbar( thresh_label, standard_name, &s_trackbar, max_trackbar, Standard_Hough);

	createTrackbar( thresh_label, "gray", &p_trackbar, max_trackbar, &blacknwhite);
	createTrackbar( "Inv?", "gray", &blacktowhite, 1, &blacknwhite);
	createTrackbar( "xup", "denoise", &xup, smoothfactor, &denoise);
	createTrackbar( "yup", "denoise", &yup, smoothfactor, &denoise);
	createTrackbar( "xdown", "denoise", &xdown, smoothfactor, &denoise);
	createTrackbar( "ydown", "denoise", &ydown, smoothfactor, &denoise);

	waitKey(0);


	findLanes();
	return 0;
}

void blacknwhite(int, void*){
	if(blacktowhite!=true){
		///try filtering out the white since we have lines anyway
		threshold(blurred, edges, p_trackbar, 255, THRESH_BINARY );
		imshow("gray", edges);
	}else{
		///try filtering out the black since we have lines anyway
		threshold(blurred, edges, p_trackbar, 255, THRESH_BINARY_INV );
		imshow("gray", edges);
	}
}

void denoise(int, void*){
	element = getStructuringElement(MORPH_RECT, Size(xup+1, yup+1), Point(-1,-1));
	dilate(edges, denoised_img, element);
	element = getStructuringElement(MORPH_RECT, Size(xdown+1, ydown+1), Point(-1,-1));
	erode(denoised_img, denoised_img, element);
	imshow("denoise", denoised_img);
}

void laneCenters(Mat rowIn, int y, int* rowDeriv, int* rowLines){
		int lastPixel=0;
		unsigned int nDeriv=0;
		unsigned int nLines =0;
		for(int i =0; i<rowIn.cols; i++){
			uchar* pixel = rowIn.ptr(0);
			if(pixel[i]!=lastPixel){
				lastPixel =  pixel[i];
				nDeriv++;
				rowDeriv[nDeriv]=i;
				//printf("got transition!  %i, %i  \n", i, lastPixel);

				if((i!=0)&&(i-rowDeriv[nDeriv-1]>(sqrt(y)+50))){
					nLines++;
					rowLines[nLines]=(i+rowDeriv[nDeriv-1])/2;
					circle(lanes, Point(rowLines[nLines], y),5,120);
				}
			}

		}
}

void findLanes(){
	denoised_img.copyTo(lanes);

	int lowHeight = denoised_img.rows-10;
	int midHeight = denoised_img.rows/3;
	int highHeight = 40;


	Mat lowerRow = denoised_img.row(lowHeight);
	int lowerRowDeriv[lowerRow.cols];
	int lowerRowLines[lowerRow.cols];

	laneCenters(lowerRow, lowHeight, lowerRowDeriv, lowerRowLines);


	Mat midRow = denoised_img.row(midHeight);
	int midRowDeriv[midRow.cols];
	int midRowLines[midRow.cols];

	laneCenters(midRow, midHeight, midRowDeriv, midRowLines);

	Mat highRow = denoised_img.row(highHeight);
	int highRowDeriv[highRow.cols];
	int highRowLines[highRow.cols];

	laneCenters(highRow, highHeight, highRowDeriv, highRowLines);

	namedWindow("lanes", CV_WINDOW_AUTOSIZE);
	imshow("lanes", lanes);


	waitKey(0);
}
