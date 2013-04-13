//============================================================================
// Name        : BlobLanesLCM.cpp
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

#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_core.hpp>
#include <blicycle/blicycle_packet_t.hpp>

#include <stdio.h>


using namespace std;
using namespace cv;

int inY = 600;
int inX = 800;
int cropHeight=0;
int cropLow=0;
int xsize;
int ysize;

Mat srcCopy, blurred, edges, element, denoised_img, lanes, accumulated;
Mat src_gray(Size(inX,inY), CV_32FC1, Scalar(0));
vector<Vec3i> lanesX;


int min_threshold = 50;
int max_trackbar = 200;
int blacktowhite = 0;
int thresh = 118;
int minLineLength = 90;
int maxLineGap = 127;
int cannyBool =1;
int accumAlpha = 36;

typedef struct _BearingInformation{
	int lock;
	float rho;
	float theta;
} BearingInformation;


blicycle::blicycle_packet_t bikepacket;
lcm::LCM globalLCM;

const char* probabilistic_name = "Probabilistic Hough Lines Demo";
char* lcmChannelIn;
char* lcmChannelOut;

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

int expectedLaneWidth = 40;

void blacknwhite(int, void*);
void denoise(int, void*);
void updateLines(int, void*);
void showLines(vector<Vec4i>, Mat);
void findLanes();
void findSlope(vector<Vec3f>&);
void fitLines(vector<Vec3f>, int, vector<int>&, vector<int>&, vector<int>&);
void reduceVector(vector<int>&);
vector<int> stripVectorRow(vector<Vec3i>, int);
int nearestElementIndex(vector<int>, int);
vector<float> stripVectorRow(vector<Vec3f>, int);
int nearestElementIndex(vector<float>, float);


class Handler{
public:
	~Handler() {}

	void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const bot_core::image_t* msg) {

		Mat src(msg->height, msg->width, CV_8UC3);
		src.data = (uchar*)&msg->data[0];

		src.copyTo(srcCopy);
		//srcCopy.adjustROI(-cropHeight, 0, 0, 0 );
		//xsize=srcCopy.cols;
		//ysize=srcCopy.rows;


		/// Pass the image to gray
		//cvtColor( srcCopy, src_gray, CV_RGB2GRAY );
		cvtColor(srcCopy,srcCopy,CV_RGB2GRAY);

		resize(srcCopy,srcCopy, Size(inX, inY));

		//downsample the image
		//pyrDown(src_gray, blurred);
		//imshow("blurred", blurred);

		//add to running average image to help blur and smooth
		accumulateWeighted(srcCopy, src_gray, accumAlpha/100.0);
		src_gray.convertTo(blurred,CV_8UC1);

		blurred.adjustROI(-cropHeight, -cropLow, 0, 0);
		xsize=blurred.cols;
		ysize=blurred.rows;

		blacknwhite(0,0);

		lowHeight = denoised_img.rows-10;
		midHeight = denoised_img.rows/3;
		highHeight = min(40, denoised_img.rows/4);

		updateLines(0,0);
		waitKey(1);
	}

};


// Print help information
void print_help() {
	printf("** Invalid syntax!\n"
			"   Usage: blicycle  <source> <output>\n"
			"       source: the LCM channel to use\n"
			"		output: the LCM channel to use\n");
}


int main(int argc, char** argv) {

	// Print a welcome message
	printf("Blicycle CV v2.0\n");

	if (argc!=3){
		print_help();
		exit(1);
	}

	lcmChannelIn = argv[1];
	lcmChannelOut = argv[2];

	if(!globalLCM.good()){
		return 1;
	}

	Handler imagehandler;

	globalLCM.subscribe(lcmChannelIn, &Handler::handleMessage, &imagehandler);
	//bot_core_image_t_subscription_t sub = bot_core_image_t_subscribe(self->subscribe_lcm, lcmChannel, on_image_frame, self);

	// Create a window to display processed images (useful for debugging)
	//namedWindow("blurred", CV_WINDOW_AUTOSIZE);
	namedWindow("gray", CV_WINDOW_AUTOSIZE);
	//namedWindow("denoise", CV_WINDOW_AUTOSIZE);
	namedWindow("lanes", CV_WINDOW_AUTOSIZE);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf( thresh_label, "Thres: %d + input", min_threshold );
	createTrackbar( thresh_label, "gray", &p_trackbar, max_trackbar, &blacknwhite);

	createTrackbar("crop top", "gray", &cropHeight, 500, &blacknwhite);
	createTrackbar("crop bottom", "gray", &cropLow, 500, &blacknwhite);

	createTrackbar("lineGap", "lanes", &maxLineGap, 500, &updateLines);
	createTrackbar("lineLength", "lanes", &minLineLength, 200, &updateLines);
	createTrackbar("Hough Thresh", "lanes", &thresh, 500, &updateLines);
	createTrackbar("Use Canny?", "lanes", &cannyBool, 1, &updateLines);

	createTrackbar( "Inv?", "gray", &blacktowhite, 1, &blacknwhite);
	createTrackbar( "Delay", "gray", &accumAlpha, 100, &updateLines);
	createTrackbar( "xup", "lanes", &xup, smoothfactor, &denoise);
	createTrackbar( "yup", "lanes", &yup, smoothfactor, &denoise);
	createTrackbar( "xdown", "lanes", &xdown, smoothfactor, &denoise);
	createTrackbar( "ydown", "lanes", &ydown, smoothfactor, &denoise);
	createTrackbar("Update Lines", "lanes", &pointless, 1, &blacknwhite);

	while (1) {
		globalLCM.handle();
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

	bikepacket.timestamp = (int64_t)time(NULL);
	bikepacket.lock=lock;   //effectively a boolean if current lane is know, may actually return some relevant value as well, but zero if unsure
	bikepacket.phi = theta;    //rotational error
	bikepacket.lane = 3;    //current lane
	bikepacket.totalLanes = 5; //total lanes in view
	bikepacket.delta = 0;  //lateral error from center of lane

	globalLCM.publish(lcmChannelOut, &bikepacket);

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

			if((i!=0)&&(i-rowDeriv[nDeriv-1]>(sqrt(y)+30))){
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

	vector<Vec3f> slopes;
	findSlope(slopes);

	reduceVector(lowerRowLines);
	reduceVector(midRowLines);
	reduceVector(highRowLines);

	fitLines(slopes, numCols, lowerRowLines, midRowLines, highRowLines);
}

void reduceVector(vector<int> &vec){
	vec.erase(std::remove(vec.begin(), vec.end(), 0), vec.end());
}

void findSlope(vector<Vec3f> &laneSlopes){

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
			//laneSlopes[r][0] = (p_lines[r][0]+p_lines[r][2])/2;  //i think its more important to refernce the bottom x and top separately, though this may cause pileups at 0
			laneSlopes[r][0] = p_lines[r][0];
			laneSlopes[r][2] = p_lines[r][2];
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


void fitLines(vector<Vec3f> slopes, int numcolumns, vector<int> &low, vector<int> &mid, vector<int> &high){
	float slope = 99999999;
	lanesX.assign(high.size(), 0);
	//printf("slope is %f, highpoints are %d\n", slope, (int)high.size());

	if(slopes.size()==0){
		printf("slopes empty!\n");
		return;}

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
			if(abs(x0-slopes[index][0])>abs(x0-slopes[r][0])){
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
				if(fabs(low.at(x2i)-x2)<spreadError*2){
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
	int xdiff = 99999;
	int probableLane=0;
	bool goodLanes=false;
	for (unsigned int i=0; i<lanesX.size(); i++){
		if(lanesX[i][1]!=0){
			line(lanes, Point(lanesX[i][0], highHeight), Point(lanesX[i][1], midHeight), Scalar(0,0,255), 10);
		}
		if(lanesX[i][2]!=0){
			line(lanes, Point(lanesX[i][1], midHeight), Point(lanesX[i][2], lowHeight), Scalar(0,0,255), 10);
			if ((abs(lanesX[i][2]-xsize/2)<xdiff)&&(lanesX[i][1]!=0)){
				xdiff=abs(lanesX[i][2]-xsize/2);
				probableLane = i;  //check zero indexing
				goodLanes = true;
			}
		}
	}
	double nearestTheta=3.1415/2;
	//int nearestLane = nearestElementIndex(stripVectorRow(lanesX,3), xsize/2); //accomplished in above if statement
	int nearestLaneLine = nearestElementIndex(stripVectorRow(slopes,3), xsize/2);
	//if((abs(lanesX[probableLane][2]-xsize/2)<abs(slopes[nearestLaneLine][2]-xsize/2))&&(lanesX.size()!=0)){
	float run = ((float)(lanesX[probableLane][1]-lanesX[probableLane][2]));
	if((probableLane!=0)&&(lanesX.size()!=0)){
		nearestTheta = atan2((lowHeight-midHeight),run);
		printf("lanes");
	}else if(slopes.size()!=0){
		nearestTheta = atan2((lowHeight-midHeight),run);
		printf("slopes");
	}
	printf("numlanes %d, angle %f, current denom %f, x1 %d x2 %d\n", lowHeight-midHeight, nearestTheta, run,lanesX[probableLane][2],lanesX[probableLane][1]);

	// Send over the data!
	publishLCM((int)lanesX.size(), 0, nearestTheta);
}

/*void reduceVec3i(vector<Vec3i> bigVector){
	for(int i=0; i<bigVector; i++){
		if(bigVector[i][0]!=0)
	}
}
*/

vector<int> stripVectorRow(vector<Vec3i> fullVector, int row){
	vector<int> output;
	output.resize(fullVector.size());
	for(unsigned int i =0; i< fullVector.size(); i++){
		output[i]=fullVector[row][i];
	}
	return output;
}

int nearestElementIndex(vector<int> searchThis, int findThis){
	int bestFit = 0;
	for(unsigned int i =0; i<searchThis.size(); i++){
		if(abs(searchThis[i]-findThis)<abs(searchThis[bestFit]-findThis)){
			bestFit = i;
		}
	}
	return bestFit;
}

vector<float> stripVectorRow(vector<Vec3f> fullVector, int row){
	vector<float> output;
	output.resize(fullVector.size());
	for(unsigned int i =0; i< fullVector.size(); i++){
		output[i]=fullVector[row][i];
	}
	return output;
}

int nearestElementIndex(vector<float> searchThis, float findThis){
	int bestFit = 0;
	for(unsigned int i =0; i<searchThis.size(); i++){
		if(abs(searchThis[i]-findThis)<abs(searchThis[bestFit]-findThis)){
			bestFit = i;
		}
	}
	return bestFit;
}
