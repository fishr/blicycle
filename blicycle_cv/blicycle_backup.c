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
#include "stdio.h"
#include "stdlib.h"

#define CAPTURE_WIDTH 160
#define CAPTURE_HEIGHT 120

void print_help() {
    printf("** Invalid syntax!\n"
           "   Usage: blicycle <capture/file> <source>\n"
           "       Capture: Use a live video stream. In\n"
           "           this case source is the camera #.\n\n"
           "       File: Read in an (AVI) file. In this\n"
           "            case data is a filename.\n");
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
    
    } else if (!strcasecmp(argv[1], "FILE")) {
        // Play from a file
	printf("   Mode: File playback\n   File: %s\n\n", argv[2]);
        capture = cvCaptureFromFile(argv[2]);
	if (!capture) {
		printf("Invalid file name!\n");
		exit(1);
        }


    } else {
        // Invalid
	print_help();
        exit(1);
    }    
   
    // List libraries and modules. IPP will show up here it if is installed. 
    char* libraries;
    char* modules;
    cvGetModuleInfo(0, (const char **) &libraries, (const char **) &modules);
    printf("   Libraries: %s\n   Modules: %s\n\n", libraries, modules);
    
    // Create a window to display processed images (useful for debugging)
    cvNamedWindow("Blicycle CV", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Input Feed", CV_WINDOW_AUTOSIZE);


    IplImage* frame;
    // Grab the first frame to get the size, so we can set up other useful 
    // temporary images
    frame = cvQueryFrame(capture);
    CvSize size = cvGetSize(frame);
    int width = size.width;
    int height = size.height;
    printf("   Capturing at:\n      Width: %d\n      Height:%d\n", width, height);

    IplImage* work = cvCloneImage(frame);
    IplImage* h = cvCreateImage(size, IPL_DEPTH_8U, 1);
    IplImage* s = cvCreateImage(size, IPL_DEPTH_8U, 1);
    IplImage* v = cvCreateImage(size, IPL_DEPTH_8U, 1);    


    // CvScalar lb = cvScalar(20, 50, 10, 0);
    // CvScalar ub = cvScalar(45, 200, 241, 255);
    CvScalar lb = cvScalar(20, 60, 10, 0);
    CvScalar ub = cvScalar(60, 255, 245, 255);

    CvMemStorage* storage = cvCreateMemStorage(0);

    while(1) {
        frame = cvQueryFrame( capture );
        if( !frame ) break;

	cvCopy(frame, work, NULL);

	cvCvtColor(work, work, CV_BGR2HSV);
	//cvSetImageROI(frame, cvRect(50, 50, 50, 50));
	//cvSet(frame, cvScalar(0, 0, 255, 0), NULL);
	///cvResetImageROI(frame);

	// cvSplit(work, h, s, v, NULL);
	cvInRangeS(work, lb, ub, h);	
	//cvCanny(h, s, 50, 200, 3);

	//CvSeq* lines = cvHoughLines2(s, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 40, 30, 10);
	//int i;
	//for(i = 0; i < lines->total; i++) {
	//	CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i);
	//	cvLine(frame, line[0], line[1], CV_RGB(255, 0, 0), 3, 8, 1);
	//}
/*
        CvSeq* lines = cvHoughLines2(	s, 
					storage, 
					CV_HOUGH_PROBABILISTIC, 
					1,
					CV_PI/180, 
					100, 
					400, 
					100	
		);
        int i;
        for(i = 0; i < lines->total; i++) {
                CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i);
                cvLine(frame, line[0], line[1], CV_RGB(255, 0, 0), 3, 8, 0);
        }


*/
	//cvSet(work, cvScalar(0, 255, 0, 0), h);

	//int y = 0;
	//for(y = 0; y <= 400; y+= 100) {
	//	cvLine(frame, cvPoint(0, y), cvPoint(600, y), cvScalar(255, 0, 0, 0), 1, 8, 0); 
	//}
	
	

	cvShowImage("Input Feed", frame);
	cvSet(frame, cvScalar(0, 255, 0, 0), h);
        cvShowImage("Blicycle CV", frame );
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow("Example2");
}
