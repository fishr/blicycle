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

#include <lcm/lcm.h>
#include <lcmtypes/bot_core.h>
#include <blicycle_packet_t.h>

#include <stdio.h>


using namespace std;
using namespace cv;

Mat srcCopy, blurred, src_gray, edges, element, denoised_img, lanes;
vector<Vec3i> lanesX;


int min_threshold = 50;
int max_trackbar = 200;
int blacktowhite = 0;
int thresh = 118;
int minLineLength = 90;
int maxLineGap = 127;
int cannyBool =1;

typedef struct _BearingInformation{
	int lock;
	float rho;
	float theta;
} BearingInformation;

typedef struct _Comp {
  lcm_t* subscribe_lcm;
  lcm_t* publish_lcm;
  BearingInformation b;
}Comp;

//blicycle_packet_t packet;

const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int p_trackbar = 178;
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

int cropHeight=0;

int expectedLaneWidth = 40;

void blacknwhite(int, void*);
void denoise(int, void*);
void updateLines(int, void*);
void showLines(vector<Vec4i>, Mat);
void findLanes();
void findSlope(vector<Vec2f>&);
void fitLines(vector<Vec2f>, int, vector<int>&, vector<int>&, vector<int>&);
void reduceVector(vector<int>&);

void on_image_frame(const lcm_recv_buf_t *rbuf, const char *channel,
		const bot_core_image_t *msg, void *user_data) {


	Comp *self = (Comp*) user_data;

	BearingInformation* bearings = &(self->b);

	Mat src(msg->height, msg->width, CV_8UC3);
	src.data = msg->data;

	src.copyTo(srcCopy);
	srcCopy.adjustROI(-cropHeight, 0, 0, 0 );


	/// Pass the image to gray
	cvtColor( srcCopy, src_gray, CV_RGB2GRAY );

	//downsample the image
	pyrDown(src_gray, blurred);
	//imshow("blurred", blurred);

	blacknwhite(0,0);

	lowHeight = denoised_img.rows-10;
	midHeight = denoised_img.rows/3;
	highHeight = min(40, denoised_img.rows/4);

	updateLines(0,0);
	waitKey(1);
}

// Print help information
void print_help() {
	printf("** Invalid syntax!\n"
			"   Usage: blicycle  <source>\n"
			"       source: the LCM channel to use\n");
}


int main(int argc, char** argv) {

	// Print a welcome message
	printf("Blicycle CV v2.0\n");

	if (argc!=2){
		print_help();
		exit(1);
	}

	char* lcmChannel = argv[1];

	//setup lcm
	Comp *self = (Comp*) calloc(1, sizeof(Comp));

	//self->publish_lcm = lcm_create(NULL);
	self->subscribe_lcm = lcm_create(NULL);


	bot_core_image_t_subscription_t * sub = bot_core_image_t_subscribe(self->subscribe_lcm, lcmChannel, on_image_frame, self);

	// Create a window to display processed images (useful for debugging)
	//namedWindow("blurred", CV_WINDOW_AUTOSIZE);
	namedWindow("gray", CV_WINDOW_AUTOSIZE);
	//namedWindow("denoise", CV_WINDOW_AUTOSIZE);
	namedWindow("lanes", CV_WINDOW_AUTOSIZE);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf( thresh_label, "Thres: %d + input", min_threshold );
	createTrackbar( thresh_label, "gray", &p_trackbar, max_trackbar, &blacknwhite);

	createTrackbar("crop", "gray", &cropHeight, 500, &blacknwhite);

	createTrackbar("lineGap", "lanes", &maxLineGap, 500, &updateLines);
	createTrackbar("lineLength", "lanes", &minLineLength, 200, &updateLines);
	createTrackbar("Hough Thresh", "lanes", &thresh, 500, &updateLines);
	createTrackbar("Use Canny?", "lanes", &cannyBool, 1, &updateLines);

	createTrackbar( "Inv?", "gray", &blacktowhite, 1, &blacknwhite);
	createTrackbar( "xup", "lanes", &xup, smoothfactor, &denoise);
	createTrackbar( "yup", "lanes", &yup, smoothfactor, &denoise);
	createTrackbar( "xdown", "lanes", &xdown, smoothfactor, &denoise);
	createTrackbar( "ydown", "lanes", &ydown, smoothfactor, &denoise);
	createTrackbar("Update Lines", "lanes", &pointless, 1, &blacknwhite);


	while (1) {
		lcm_handle(self->subscribe_lcm);
	}

	//destroyWindow("blurred");
	destroyWindow("gray");
	//destroyWindow("denoise");
	destroyWindow("lanes");

}

/**
 * A helper method to publish data over a socket.
 */
void publishLCM(int32_t lock, double rho, double theta) {
/*
	packet->lock=lock;
	packet->phi = 0;
	packet->lane = 3;
	packet->totalLanes = 5;
	packet->delta = 0;
	*/

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
	denoise(0,0);
}

void updateLines(int, void*){
	findLanes();

	imshow("lanes", lanes);
}

void denoise(int, void*){
	element = getStructuringElement(MORPH_RECT, Size(xup+1, yup+1), Point(-1,-1));
	dilate(edges, denoised_img, element);
	element = getStructuringElement(MORPH_RECT, Size(xdown+1, ydown+1), Point(-1,-1));
	erode(denoised_img, denoised_img, element);
	//imshow("denoise", denoised_img);
}

void showLines(vector<Vec4i> reducedLines, Mat screen){
	for( size_t i = 0; i < reducedLines.size(); i++)
	{
		Vec4i l = reducedLines[i];
		line( screen, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
	}
}

void laneCenters(Mat imgSrc, int y, vector<int> &rowLines){
	//takes a b&w image row and processes each value to find transitions, then marks the middle
	//of successive white to black -> black to white transitions and stores the x values in rowlines

	//todo make this account both for image slant and adjust better for image height

	Mat rowIn = imgSrc.row(y);
	int rowDeriv[rowIn.cols];

	int lastPixel=255;
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


	unsigned int numCols = denoised_img.cols;
	vector<int> lowerRowLines;
	lowerRowLines.resize(numCols);

	laneCenters(denoised_img, lowHeight, lowerRowLines);


	vector<int> midRowLines;
	midRowLines.resize(numCols);

	laneCenters(denoised_img, midHeight, midRowLines);

	vector<int> highRowLines;
	highRowLines.resize(numCols);

	laneCenters(denoised_img, highHeight, highRowLines);

	vector<Vec2f> slopes;
	findSlope(slopes);

	reduceVector(lowerRowLines);
	reduceVector(midRowLines);
	reduceVector(highRowLines);

	fitLines(slopes, numCols, lowerRowLines, midRowLines, highRowLines);
}

void reduceVector(vector<int> &vec){
	vec.erase(std::remove(vec.begin(), vec.end(), 0), vec.end());
}

void findSlope(vector<Vec2f> &laneSlopes){

	//todo could use this to evaluate slope then rotate image?

	//todo also could find the slopes around sets of points to better account for local slope

	vector<Vec4i> p_lines;

	//double slope=0;
/*
	while((p_lines.size()<4)||(p_lines.size()>20)){
		p_lines.clear();
		if(p_lines.size()<4){
			thresh-=10;
		}else{
			thresh+=10;
		}
		*/
	if(cannyBool)
		Canny(denoised_img, denoised_img, 100, 200);
	HoughLinesP( denoised_img, p_lines, 1, CV_PI/180, thresh, minLineLength, maxLineGap);
	//}
	//smooth by slope and use identified slope as seed for lines in lanes

	showLines(p_lines, lanes);
	/*unsigned int slopeCounter = 0;
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

	if(slopeCounter!=0)
		slope = slope/(float) slopeCounter;
	return slope;
	*/
	if(p_lines.size()!=0){
		laneSlopes.resize((int)p_lines.size());
		for(int r=0;r<(int)p_lines.size();r++){
			laneSlopes[r][0] = (p_lines[r][0]+p_lines[r][2])/2;
			float slope = 0.000001;
			int numer = (p_lines[r][1]-p_lines[r][3]);
			if (numer!=0){
				int denom = (p_lines[r][0]-p_lines[r][2]);
				if (denom != 0){
					slope = (numer)/((float)denom);
				}else{
					slope = 9999999;
				}
			}
			laneSlopes[r][1] = slope;
		}
	}

}


void fitLines(vector<Vec2f> slopes, int numcolumns, vector<int> &low, vector<int> &mid, vector<int> &high){
	float slope = 99999999;
	lanesX.assign(high.size(), 0);
	//printf("slope is %f, highpoints are %d\n", slope, (int)high.size());

	if(high.size()==0){
		printf("numlanes %d\n", (int)lanesX.size()-1);
		return;}

	int laneCounter=0;

	for (unsigned int i=0; i<(high.size()); i++){
		int x0 = high.at(i);
		bool gotx1=false;
		bool gotx2=false;
		int index=0;
		for(int r = 0; r<(int)slopes.size(); r++){
			if(min(abs(x0-slopes[index][0]),abs(x0-slopes[r][0]))==abs(x0-slopes[r][0])){
				index =r;
			}
		}
		slope = slopes[index][1];
		float x1 = ((midHeight-highHeight)/slope)+x0;
		float x2 = ((lowHeight-highHeight)/slope)+x0;

		//printf("high %d,  mid %f, low %f\n", x0, x1, x2);

		for(unsigned int x1i=0; x1i<(mid.size()); x1i++){
			if (!gotx1){
				if(fabs(mid.at(x1i)-x1)<spreadError){ //is the point close enough to the average slope?
					lanesX[laneCounter][0]=x0;
					lanesX[laneCounter][1]=mid.at(x1i);
					gotx1=true;
					//printf("middle match %d with %d\n", high.at(i), mid.at(x1i));

				}
			}else{
				if(fabs(mid.at(x1i)-x1)<fabs(lanesX[laneCounter][1]-x1)){//is this point better than a previous one?
					lanesX[laneCounter][1]=mid.at(x1i);
				}
			}
		}

		for(unsigned int x2i=0; x2i<(low.size()); x2i++){//same as above
			if (!gotx2){
				if(fabs(low.at(x2i)-x2)<spreadError){
					lanesX[laneCounter][0]=x0;
					lanesX[laneCounter][2]=low.at(x2i);
					gotx2=true;
					//printf("lower match %d with %d\n", high.at(i), low.at(x2i));
				}
			}else{
				if(fabs(low.at(x2i)-x2)<fabs(lanesX[laneCounter][2]-x2)){
					lanesX[laneCounter][2]=low.at(x2i);
				}
			}
		}
		if(gotx1||gotx2){
			//printf("gotalane\n");
			laneCounter++;
		}
	}

	for(int foo=0; foo<(((int)high.size())-(laneCounter+1));foo++){
		//printf("poof!\n");
		lanesX.pop_back();
	}

	for (unsigned int i=0; i<lanesX.size(); i++){
		if(lanesX[i][1]!=0){
			line(lanes, Point(lanesX[i][0], highHeight), Point(lanesX[i][1], midHeight), Scalar(0,0,255), 10);
		}
		if(lanesX[i][2]!=0){
			line(lanes, Point(lanesX[i][0], highHeight), Point(lanesX[i][2], lowHeight), Scalar(0,0,255), 10);
		}
	}

	//printf("numlanes %d\n", (int)lanesX.size()-1);

	// Send over the data!
	publishLCM((int)lanesX.size(), 2, slope);
}
