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

#include "cv.h"
#include "highgui.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

//#define CAPTURE_WIDTH 160
//#define CAPTURE_HEIGHT 120
#define CAPTURE_WIDTH 320
#define CAPTURE_HEIGHT 240

#define SOCKET_PORT_CONTROLLER 20000

typedef struct {
	int lock;
	float rho;
	float theta;
} BearingInformation;

// Global variables
int width;
int height;
	    
IplImage* work;
IplImage* h;
IplImage* s;
IplImage* v;

CvScalar lb;
CvScalar ub;

// Socket information
int sockfd = -1;

// State information
double rho_best_last;
double theta_best_last;
double alpha = 0.0;	// How much to weight the last sample.

CvMemStorage* storage;

/**
 * A helper method to publish data over a socket.
 */
void publishOverSocket(int lock, double rho, double theta) {
	char buffer[256];
	
	if (sockfd != -1) {
		snprintf(buffer, 256, "%d, %f, %f\n", lock, rho, theta);
		int n = send(sockfd, buffer, strlen(buffer), 0); 
		if (n < 0) {
			printf("Error writing to socket!\n");
			sockfd = -1;
		}
	}
	
}

void getBearingInformation(CvPoint* p1, CvPoint* p2, BearingInformation* bearings) {
			// Normalize the point w.r.t the screen resolution 
			// (this will allow the code to operate successfully at different screen
			// capture resolutions for more computational efficiency on small computers)
			float x1 = (float) p1->x / width;
			float y1 = (float) (height - p1->y) / width;
			float x2 = (float) p2->x / width;
			float y2 = (float) (height - p2->y) / width;
			
			// Convert this line segment to rho, theta form with respect to 
			// a center point from the bottom of the screen. This transformation
			// will make it such that line segments along the same line will all
			// map to the same line. Also, parallel lines will map to the same theta 
			// (but different rho).
			float deltaX_l = (x2 - x1);
			float deltaY_l = (y2 - y1);
			float L = sqrt(deltaX_l*deltaX_l + deltaY_l*deltaY_l);
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
	double delta_last = rho_coeff * (rho - rho_best_last)*(rho - rho_best_last) + theta_coeff * (theta - theta_best_last) * (theta - theta_best_last);


	cost += delta_weighting * delta_last;

	return cost;
}

void processFrame(IplImage* frame, BearingInformation* bearings) {
	// Convert to the HSV color space
	cvCopy(frame, work, NULL);
	cvCvtColor(frame, frame, CV_BGR2HSV);

	// cvSplit(work, h, s, v, NULL);
	cvInRangeS(frame, lb, ub, h);	
	cvCanny(h, s, 50, 200, 3);

		

	CvSeq* lines = cvHoughLines2(	s, 
					storage, 
					CV_HOUGH_PROBABILISTIC, 
					1,
					CV_PI/180, 
					50, 
					(int) (height / 480.0f * 200.0f), 
					(int) (height / 480.0f * 100.0f)	
	);

	// Now filter and select the best one
	CvPoint* best_line = NULL;
	float best_cost = 1000000.0f;
	int i;

	// Assign a cost to each line, and select the one with the minimum.	

	for(i = 0; i < lines->total; i++) {
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


int main( int argc, char** argv ) {

	// Print a welcome message
	printf("Blicycle CV v1.0\n");

	// Based on the command line arguments to this executable, capture either from 
	// a data file or from a live video stream
	if (argc != 3) {
		print_help();
		exit(1);
	}
    
	CvCapture* capture;
	IplImage* image = NULL;
	CvSize size;

	if (!strcasecmp(argv[1], "CAPTURE")) {
		// Capture live video
		int video_source = atoi(argv[2]);
		capture = cvCaptureFromCAM(video_source);
		if (!capture) {
			printf("Couldn't capture video!\n");
		}
        
		// Now set some additional capture settings
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, CAPTURE_WIDTH);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, CAPTURE_HEIGHT);
		width = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		height = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		size.width = width;
		size.height = height;
    
	} else if (!strcasecmp(argv[1], "VIDEOFILE")) {
		// Play from a file
		printf("   Mode: File playback\n   File: %s\n\n", argv[2]);
		capture = cvCaptureFromFile(argv[2]);
		if (!capture) {
			printf("Invalid video file name!\n");
			exit(1);
		}

		width = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		height = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		size.width = width;
		size.height = height;

	} else if (!strcasecmp(argv[1], "IMAGEFILE")) {
		// Play from a file
		printf("   Mode: Image process\n   File: %s\n\n", argv[2]);
		image = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);
		if (!image) {
			printf("Invalid image file name!\n");
			exit(1);
		}
		// Convert to the HSV color space	
		//cvCvtColor(image, image, CV_RGB2BGR);

		width = image->width;
		height = image->height;
		size.width = width;
		size.height = height;

	} else {
		// Invalid
		print_help();
		exit(1);
	}    
   

	// Try to connect to the controller socket server!
	struct hostent * server;
	struct sockaddr_in server_addr;
	const char SERVER_NAME[] = "localhost";
	
	server = gethostbyname(SERVER_NAME);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Couldn't create socket\n");
		exit(1);
 	}

	server_addr.sin_family = AF_INET;     
        server_addr.sin_port = htons(SOCKET_PORT_CONTROLLER);   
        server_addr.sin_addr = *((struct in_addr *)server->h_addr);
        bzero(&(server_addr.sin_zero),8); 
	
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1)  {
		perror("Could not connect to controller socket!\n");
		exit(1);
        } else {
		printf("Successfully connected to controller socket!\n");
	}


	// List libraries and modules. IPP will show up here it if is installed. 
	char* libraries;
	char* modules;
	cvGetModuleInfo(0, (const char **) &libraries, (const char **) &modules);
	printf("   Libraries: %s\n   Modules: %s\n\n", libraries, modules);
    
	// Create a window to display processed images (useful for debugging)
	cvNamedWindow("Blicycle CV", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Input Feed", CV_WINDOW_AUTOSIZE);

	printf("   Capturing at:\n      Width: %d\n      Height:%d\n", width, height);


	work = cvCreateImage(size, IPL_DEPTH_8U, 3);
	h = cvCreateImage(size, IPL_DEPTH_8U, 1);
	s = cvCreateImage(size, IPL_DEPTH_8U, 1);
	v = cvCreateImage(size, IPL_DEPTH_8U, 1);    

	// CvScalar lb = cvScalar(20, 50, 10, 0);
	// CvScalar ub = cvScalar(45, 200, 241, 255);
	lb = cvScalar(20, 60, 10, 0);
	ub = cvScalar(60, 255, 245, 255);

	storage = cvCreateMemStorage(0);

	BearingInformation bearings;

	while(1) {
		IplImage* frame;
		if (!image) {
			frame = cvQueryFrame(capture);
			if( !frame ) break;
		} else {
			frame = image;
		}
		

		// Process the frame
		processFrame(frame, &bearings);
		// Send over the data!
		publishOverSocket(bearings.lock, bearings.rho, bearings.theta);

		//cvSet(work, cvScalar(0, 255, 0, 0), h);
		cvShowImage("Input Feed", frame);
		// cvSet(frame, cvScalar(0, 255, 0, 0), h);
		cvShowImage("Blicycle CV", h );

		//cvReleaseImage(&work);
		if (!image) {
			char c = cvWaitKey(33);
			if( c == 27 ) break;
		} else {
			char c = cvWaitKey(0);
			break;
		}
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Input Feed");
	cvDestroyWindow("Blicycle CV");

}
