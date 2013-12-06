//============================================================================
// Name        : BlobLanesLCM.cpp
// Author      : Ryan Fish
// Version     :
// Copyright   : MIT License
// Description :
//============================================================================

#include "config.hpp"

#include "contourUtils.hpp"
#include "gui.hpp"
#include "handler.hpp"
#include "lanes.hpp"
#include "lcmconfig.hpp"
#include "vectorhelp.hpp"

#include <iostream>

using namespace std;
using namespace cv;

int inY = 600;
int inX = 800;
int cropHeight=170;
int cropLow=0;
int xsize;
int ysize;

bool calibrated = false;

Mat srcCopy, blurred, edges, element, denoised_img, lanes, accumulated, map1, map2;
Mat src_gray(Size(inX,inY), CV_32FC1, Scalar(0));
vector<Vec3i> lanesX;


int spreadError = 50;

int lowHeight;
int midHeight;
int highHeight;

int gray_thresh = 140;

int expectedLaneWidth = 40;
int physicalLaneWidthInch = 48;

void useImage(Mat);
void calibrateCameraConsts(int, int);
void blacknwhite(int, void*);
void denoise(int, void*);
void updateLines(int, void*);
void showLinesBW(vector<Vec4i>, Mat);
void showLines(vector<Vec4i>, Mat);
void findLanes();
void findSlope(vector<Vec3f>&);
void fitLines(vector<Vec3f>, int, vector<int>&, vector<int>&, vector<int>&);

LcmEngine lcmEngine;

void calibrateCameraConsts(int x, int y){
	Size imageSize(x, y);
	initUndistortRectifyMap(cameraMatrix, distMatrix, Mat(),
	    //getOptimalNewCameraMatrix(cameraMatrix, distMatrix, imageSize, 1),
		cameraMatrix,
	    imageSize, CV_16SC2, map1, map2);
}



int main(int argc, char** argv) {

	// Print a welcome message
	printf("Blicycle CV v2.0\n");

	if (argc!=3){
		print_help();
		exit(1);
	}

	lcmEngine.init(argv[1], argv[2]);

	if(!lcmEngine.good()){
		return 1;
	}

	lcmEngine.subscribe();
	//bot_core_image_t_subscription_t sub = bot_core_image_t_subscribe(self->subscribe_lcm, lcmChannel, on_image_frame, self);

	// Create a window to display processed images (useful for debugging)
	//namedWindow("blurred", CV_WINDOW_AUTOSIZE);
	//namedWindow("denoise", CV_WINDOW_AUTOSIZE);
	namedWindow("lanes", CV_WINDOW_AUTOSIZE);

	namedWindow("test", CV_WINDOW_AUTOSIZE);
	namedWindow("raw", CV_WINDOW_AUTOSIZE);

	/// Create Trackbars for Thresholds
	createTrackbar("crop top", "lanes", &cropHeight, 500, &blacknwhite);
	createTrackbar("crop bottom", "lanes", &cropLow, 500, &blacknwhite);

	createTrackbar("lineGap", "lanes", &maxLineGap, 500, &updateLines);
	createTrackbar("lineLength", "lanes", &minLineLength, 200, &updateLines);
	createTrackbar("Hough Thresh", "lanes", &thresh, 500, &updateLines);

	createTrackbar( "xup", "lanes", &xup, smoothfactor, &denoise);
	createTrackbar( "yup", "lanes", &yup, smoothfactor, &denoise);
	createTrackbar( "xdown", "lanes", &xdown, smoothfactor, &denoise);
	createTrackbar( "ydown", "lanes", &ydown, smoothfactor, &denoise);

	while (1) {
		lcmEngine.handle();
		if(lcmEngine.imageHandler.newImage())
			useImage(lcmEngine.imageHandler.src);
	}

	//destroyWindow("blurred");
	destroyWindow("gray");
	//destroyWindow("denoise");
	destroyWindow("lanes");

	destroyWindow("test");
	destroyWindow("raw");
}

void useImage(Mat src){

	if(!calibrated)
		calibrateCameraConsts(lcmEngine.imageHandler.width, lcmEngine.imageHandler.height);
	calibrated = true;

	cv::imshow("raw", src);

	cv::remap(src, srcCopy, map1, map2, cv::INTER_LINEAR);

	/// Pass the image to gray
	//cvtColor( srcCopy, src_gray, CV_RGB2GRAY );
	cvtColor(srcCopy,srcCopy,CV_RGB2GRAY);

	cv::resize(srcCopy,src_gray, cv::Size(inX, inY));

	//downsample the image
	//pyrDown(src_gray, blurred);
	//imshow("blurred", blurred);

	src_gray.convertTo(blurred,CV_8UC1);

	blurred.adjustROI(-cropHeight, -cropLow, 0, 0);
	xsize=blurred.cols;
	ysize=blurred.rows;

	blacknwhite(0,0);

	lowHeight = denoised_img.rows-10;
	midHeight = denoised_img.rows/3;
	highHeight = min(40, denoised_img.rows/4);

	updateLines(0,0);
	cv::waitKey(1);
}

void blacknwhite(int, void*){
	cv::equalizeHist(blurred, blurred);

		///try filtering out the white since we have lines anyway
		threshold(blurred, edges, gray_thresh, 255, THRESH_BINARY );

	denoise(0,0);
}

void updateLines(int, void*){
	//findLanes();

	//starts here

	RNG rng(12345);


	  vector<vector<Point> > contours;
	  vector<Vec4i> hierarchy;
		vector<Vec4i> p_lines;

	  /// Find contours
		Mat lines(denoised_img.size(), CV_8UC1);

		Canny(denoised_img, lines, 100, 200);
		//HoughLinesP( denoised_img, p_lines, 1, CV_PI/180, thresh, minLineLength, maxLineGap);
		//smooth by slope and use identified slope as seed for lines in lanes
		//showLinesBW(p_lines, lines);
		imshow("test", lines);

	  findContours( lines, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );
	  cvtColor(denoised_img, lanes, CV_GRAY2BGR);
	  ContourUtils::processContours(contours);

	  /// Draw contours
	  Mat drawing = Mat::zeros( denoised_img.size(), CV_8UC3 );
	  for( unsigned i = 0; i< contours.size(); i++ )
	     {
	       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	       drawContours( lanes, contours, i, color, -2, 4);//, hierarchy);
	     }

	  //ends here

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

void showLinesBW(vector<Vec4i> reducedLines, Mat screen){
	for( size_t i = 0; i < reducedLines.size(); i++)
	{
		Vec4i l = reducedLines[i];
		line( screen, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 3, CV_AA);
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
	if((goodLanes==true)&&(lanesX.size()!=0)){
		nearestTheta = atan2((lowHeight-midHeight),run);
		//printf("lanes");
	}else if(slopes.size()!=0){
		nearestTheta = atan2((lowHeight-midHeight),((float)(slopes[nearestLaneLine][2]-slopes[nearestLaneLine][1])));
		//printf("slopes");
	}
	//printf("numlanes %d, angle %f, current denom %f, x1 %d x2 %d\n", lowHeight-midHeight, nearestTheta, run,lanesX[probableLane][2],lanesX[probableLane][1]);

	// Send over the data!
	lcmEngine.publishLCM((int)lanesX.size(), 0, nearestTheta);
}

