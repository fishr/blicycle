/**
 * Blicycle Computer Vision Module
 *
 * This module allows the blicycle to navigate.
 * Its job is to estimate the position of the bicycle
 * with respect to the track using computer vision.
 * Our approach is to follow the grass-track border
 * along the inner edge of the track.
 *
 * This module outputs a stream of data in the form
 * of (deltaX, deltaTheta, stable) to the blicycle controller
 * (which runs as a separate process). The blicycle
 * controller than uses this information to steer
 * the blicycle on course.
 *
 * Code written by Steve Levine, sjlevine@mit.edu
 * and released under the MIT license.
 */

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cvaux.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <getopt.h>
#include <lcm/lcm.h>
#include <lcmtypes/bot_core.h>

#define CAPTURE_WIDTH 320
#define CAPTURE_HEIGHT 240

using namespace std;
using namespace cv;

typedef struct {
	int lock;
	float rho;
	float theta;
} BearingInformation;

typedef struct _Comp {
  lcm_t* subscribe_lcm;
  lcm_t* publish_lcm;
  BearingInformation b;
}Comp;


int firstFrame = 1;

IplImage* frame;
IplImage* work;
IplImage* h;
IplImage* s;
IplImage* v;

CvSize size;

CvScalar lb;
CvScalar ub;

// State information
double rho_best_last;
double theta_best_last;
double alpha = 0.0; // How much to weight the last sample.

CvMemStorage* storage;

/**
 * A helper method to publish data over a socket.
 */
void publishLCM(int lock, double rho, double theta) {
	//TODO  make the type needed here to send these things via LCM

}

void getBearingInformation(CvPoint* p1, CvPoint* p2,
		BearingInformation* bearings) {
	// Normalize the point w.r.t the screen resolution 
	// (this will allow the code to operate successfully at different screen
	// capture resolutions for more computational efficiency on small computers)


	float x1 = (float) p1->x / size.width;
	float y1 = (float) (size.height - p1->y) / size.width;
	float x2 = (float) p2->x / size.width;
	float y2 = (float) (size.height - p2->y) / size.width;

	// Convert this line segment to rho, theta form with respect to 
	// a center point from the bottom of the screen. This transformation
	// will make it such that line segments along the same line will all
	// map to the same line. Also, parallel lines will map to the same theta 
	// (but different rho).
	float deltaX_l = (x2 - x1);
	float deltaY_l = (y2 - y1);
	float L = sqrt(deltaX_l * deltaX_l + deltaY_l * deltaY_l);
	float deltaX_c = (x2 - 0.5f);
	float deltaY_c = (y2 - 0.0f);

	//printf("%f\n", L);
	float rho = (-deltaY_l * deltaX_c + deltaX_l * deltaY_c) / L;
	float theta;
	if (deltaX_l != 0.0f) {
		theta = atan(deltaY_l / deltaX_l);
	} else {
		theta = 3.1415925535f / 2.0f;
	}

	bearings->lock = 1;
	bearings->rho = rho;
	bearings->theta = theta;
}

double costOfLine(CvPoint* line) {
	double cost = 0.0;
	BearingInformation bearings;

	// Convert this line to rho-theta form
	CvPoint p1 = line[0];
	CvPoint p2 = line[1];
	getBearingInformation(&line[0], &line[1], &bearings);
	double rho = bearings.rho;
	double theta = bearings.theta;

	// Bias to lower points on the screen.
	//cost += -(p1.y + p2.y) / 2.0;
	cost += bearings.rho;

	// Attempt to favor spatio-temporal consistency by favoring
	// line segments close to the last one. This will favor
	// non-inertial tracking.
	double rho_coeff = 1.0;
	double theta_coeff = 1.0;
	double delta_weighting = 20.0;
	double delta_last = rho_coeff * (rho - rho_best_last)
			* (rho - rho_best_last)
			+ theta_coeff * (theta - theta_best_last)
					* (theta - theta_best_last);

	cost += delta_weighting * delta_last;

	return cost;
}

void on_image_frame(const lcm_recv_buf_t *rbuf, const char *channel,
		const bot_core_image_t *msg, void *user_data) {
	Comp *self = (Comp*) user_data;

	BearingInformation* bearings = &(self->b);
	printf("got an image\n");

	Mat img_lcm = Mat::zeros(msg->height, msg->width, CV_8UC3); // h,w
	img_lcm.data = msg->data;
	if(firstFrame){
		size.width = (int) msg->width;
		size.height = (int) msg->height;

		//v = cvCreateImage(size, IPL_DEPTH_8U, 1);

		firstFrame =0;
	}

	h = cvCreateImage(size, IPL_DEPTH_8U, 1);
	s = cvCreateImage(size, IPL_DEPTH_8U, 1);

	frame = new IplImage(img_lcm);

	// Convert to the HSV color space
	cvCvtColor(frame, frame, CV_BGR2HSV);


	// cvSplit(work, h, s, v, NULL);
	cvInRangeS(frame, lb, ub, h);
	cvCanny(h, s, 50, 200, 3);

	CvSeq* lines = cvHoughLines2(s, storage, CV_HOUGH_PROBABILISTIC, 1,
			CV_PI / 180, 50, (int) (size.height / 480.0f * 200.0f),
			(int) (size.height / 480.0f * 100.0f));

	// Now filter and select the best one
	CvPoint* best_line = NULL;
	float best_cost = 1000000.0f;
	int i;

	// Assign a cost to each line, and select the one with the minimum.	
	for (i = 0; i < lines->total; i++) {
		CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i);
		double cost = costOfLine(line);

		if (cost < best_cost) {
			best_cost = cost;
			best_line = line;
		}
	}
	// Draw the best line
	if (best_line) {
		CvPoint p1 = best_line[0];
		CvPoint p2 = best_line[1];
		//cvLine(frame, p1, p2, CV_RGB(0, 0, 0), 20, 8, 0);

		getBearingInformation(&p1, &p2, bearings);

		printf("%f, %f\n", bearings->rho, bearings->theta);

		// Store some state information
		alpha = 1.0;
		rho_best_last = bearings->rho;
		theta_best_last = bearings->theta;

	} else {
		// There are no lines! Don't have a lock.
		bearings->lock = 0;
		bearings->rho = 0.0f;
		bearings->theta = 0.0f;

		alpha *= 0.8;
	}

	//cvSet(frame, cvScalar(0, 255, 0, 0), h);
	cvReleaseImage(&s);

	cvShowImage("Input Feed", frame);
	cvWaitKey(30);
}

// Print help information
void print_help() {
	printf("** Invalid syntax!\n"
			"   Usage: blicycle <capture/videofile/imagefile> <source>\n"
			"       Capture: Use a live video stream. In\n"
			"           this case source is the camera #.\n\n"
			"       VideoFile: Read in an (AVI) file. In this\n"
			"            case data is a filename.\n"
			"       ImageFile: Read in an image file, and process\n"
			"             it (not a video).\n");
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


	// List libraries and modules. IPP will show up here it if is installed. 
	char* libraries;
	char* modules;
	cvGetModuleInfo(0, (const char **) &libraries, (const char **) &modules);
	printf("   Libraries: %s\n   Modules: %s\n\n", libraries, modules);

	// Create a window to display processed images (useful for debugging)
	cvNamedWindow("Blicycle CV", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Input Feed", CV_WINDOW_AUTOSIZE);

	lb = cvScalar(20, 60, 10, 0);
	ub = cvScalar(60, 255, 245, 255);

	storage = cvCreateMemStorage(0);


	while (1) {
		lcm_handle(self->subscribe_lcm);

		// Send over the data!
		//publishLCM(self->b.lock, self->b.rho, self->b.theta);

		cvShowImage("Blicycle CV", h);
		cvWaitKey(30);
		cvReleaseImage(&h);

	}
	if(frame)
		cvReleaseImage(&frame);
	cvDestroyWindow("Input Feed");
	cvDestroyWindow("Blicycle CV");

}
