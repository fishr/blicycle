//============================================================================
// Name        : BlobLanes.cpp
// Author      : Ryan Fish
// Version     :
// Copyright   : MIT License
// Description : Hello World in C++, Ansi-style
//hours:  5
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

int spreadError = 50;

int lowHeight;
int midHeight;
int highHeight;

int pointless;


int expectedLaneWidth = 40;

void blacknwhite(int, void*);
void denoise(int, void*);
void updateLines(int, void*);
void showLines(vector<Vec4i>, Mat);
void findLanes();
float findSlope();
bool isZero(int);
void fitLines(float, int, vector<int>&, vector<int>&, vector<int>&);

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

	lowHeight = denoised_img.rows-10;
	midHeight = denoised_img.rows/3;
	highHeight = 40;

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
	createTrackbar("Update Lines", "denoise", &pointless, 1, &updateLines);



	waitKey(0);

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

void updateLines(int, void*){
	findLanes();

	namedWindow("lanes", CV_WINDOW_AUTOSIZE);
	imshow("lanes", lanes);
}

void denoise(int, void*){
	element = getStructuringElement(MORPH_RECT, Size(xup+1, yup+1), Point(-1,-1));
	dilate(edges, denoised_img, element);
	element = getStructuringElement(MORPH_RECT, Size(xdown+1, ydown+1), Point(-1,-1));
	erode(denoised_img, denoised_img, element);
	imshow("denoise", denoised_img);
}

void showLines(vector<Vec4i> reducedLines, Mat screen){
	for( size_t i = 0; i < reducedLines.size(); i++)//p_lines.size(); i++ )
	     {
	       Vec4i l = reducedLines[i];//p_lines[i];
	       line( screen, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
	     }
}

void laneCenters(Mat rowIn, int y, int* rowDeriv, vector<int> &rowLines){
		int lastPixel=0;
		unsigned int nDeriv=0;
		for(int i =0; i<rowIn.cols; i++){
			uchar* pixel = rowIn.ptr(0);
			if(pixel[i]!=lastPixel){
				lastPixel =  pixel[i];
				nDeriv++;
				rowDeriv[nDeriv]=i;
				//printf("got transition!  %i, %i  \n", i, lastPixel);

				if((i!=0)&&(i-rowDeriv[nDeriv-1]>(sqrt(y)+50))){
					rowLines.push_back((i+rowDeriv[nDeriv-1])/2);
					circle(lanes, Point(rowLines.back(), y),5,Scalar(0,255,0));
				}
			}

		}
}

void findLanes(){
	cvtColor(denoised_img, lanes, CV_GRAY2BGR);



	Mat lowerRow = denoised_img.row(lowHeight);
	unsigned int numCols = lowerRow.cols;
	int lowerRowDeriv[numCols];
	std::vector<int> lowerRowLines;
	lowerRowLines.assign(numCols, 0);

	laneCenters(lowerRow, lowHeight, lowerRowDeriv, lowerRowLines);


	Mat midRow = denoised_img.row(midHeight);
	int midRowDeriv[numCols];
	vector<int> midRowLines;
	midRowLines.assign(numCols, 0);

	laneCenters(midRow, midHeight, midRowDeriv, midRowLines);

	Mat highRow = denoised_img.row(highHeight);
	int highRowDeriv[numCols];
	std::vector<int> highRowLines;
	highRowLines.assign(numCols, 0);

	laneCenters(highRow, highHeight, highRowDeriv, highRowLines);

	float rhoslope = findSlope();

	lowerRowLines.erase(std::remove(lowerRowLines.begin(), lowerRowLines.end(), 0), lowerRowLines.end());
	midRowLines.erase(std::remove(midRowLines.begin(), midRowLines.end(), 0), midRowLines.end());
	highRowLines.erase(std::remove(highRowLines.begin(), highRowLines.end(), 0), highRowLines.end());

	fitLines(rhoslope, numCols, lowerRowLines, midRowLines, highRowLines);
}

float findSlope(){
	vector<Vec4i> p_lines;

	int thresh = 385;

	double slope=0;

	while((p_lines.size()<4)||(p_lines.size()>20)){
		p_lines.clear();
		if(p_lines.size()<4){
			thresh-=10;
		}else{
			thresh+=10;
		}
		HoughLinesP( denoised_img, p_lines, 1, CV_PI/180, thresh, 30, 200);
	}
	//smooth by slope and use identified slope as seed for lines in lanes

	showLines(p_lines, lanes);
	unsigned int slopeCounter = 0;
	for(unsigned int i=0; i<p_lines.size();i++){
		int numer = (p_lines[i][1]-p_lines[i][3]);
		if (numer!=0){
			int denom = (p_lines[i][0]-p_lines[i][2]);
			if (denom != 0){
				slope += (numer)/((float)denom);
				slopeCounter++;
			}else{
				slope += 9999999;
				slopeCounter++;
			}
			//printf("interslope is %f, numer is %d, denom is %d, i is %u of %d, p_lines %d, %d, %d, %d\n", slope, numer, denom, i, (int)p_lines.size(), p_lines[i][0], p_lines[i][1], p_lines[i][2], p_lines[i][3]);
		}
	}

	slope = slope/(float) slopeCounter;
	return slope;
}

bool isZero (int i)
{
    return i == 0;
}

void fitLines(float slope, int numcolumns, vector<int> &low, vector<int> &mid, vector<int> &high){

	vector<Vec3i> lanes;
	lanes.assign(high.size(), 0);
	printf("slope is %f\n", slope);

	int laneCounter=0;

	for (unsigned int i=0; i<(high.size()); i++){
		int x0 = high.at(i);
		float x1 = ((midHeight-highHeight)/slope)+x0;
		float x2 = ((lowHeight-highHeight)/slope)+x0;
		bool gotx1=false;
		bool gotx2=false;

		//printf("high %d,  mid %f, low %f\n", x0, x1, x2);

		for(unsigned int x1i=0; x1i<(mid.size()); x1i++){
			if (!gotx1){
				if(fabs(mid.at(x1i)-x1)<spreadError){
					lanes[laneCounter][0]=x0;
					lanes[laneCounter][1]=mid.at(x1i);
					gotx1=true;
					//printf("middle match %d with %d\n", high.at(i), mid.at(x1i));

				}
			}
		}
		for(unsigned int x2i=0; x2i<(low.size()); x2i++){
			if (!gotx2){
				if(fabs(low.at(x2i)-x2)<spreadError){
					lanes[laneCounter][0]=x0;
					lanes[laneCounter][2]=low.at(x2i);
					gotx2=true;
					//printf("lower match %d with %d\n", high.at(i), low.at(x2i));
				}
			}
		}
		if(gotx1||gotx2){
			laneCounter++;
		}
	}


}
