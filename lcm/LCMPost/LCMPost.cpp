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

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "lcm/lcm.h"
//#include "bot_core/bot_core.h"
#include "lcmtypes/bot_core.h"
#include <ctime>


// Global variables
bot_core_image_t bot_image;


using namespace std;
using namespace cv;
typedef struct _Comp {
  lcm_t* subscribe_lcm;
  lcm_t* publish_lcm;
  int counter;
}Comp;



// Print help information
void print_help() {
    printf("** Invalid syntax!\n"
           "   Usage: LCMPost <capture/avi/image> <src> <lcmChannel>\n"
           "       Capture: Use a live video stream. In\n"
           "           this case source is the camera #.\n\n"
           "       avi: Read in an (AVI) file. In this\n"
    		"            case data is a filename.\n"
    		"       Image: Read in an image file, and process\n"
    		"             it (not a video).\n"
    		"       lcmChannel: name of channel\n");
}


int main( int argc, char** argv ) {

	// Print a welcome message
	printf("LCM Post\n");

	// Based on the command line arguments to this executable, capture either from
	// a data file or from a live video stream
	if (argc != 4) {
		print_help();
		exit(1);
	}

	char* lcmChannel1 = argv[3];

	VideoCapture capture;
	Mat image;

	if (!strcasecmp(argv[1], "CAPTURE")) {
		// Capture live video
		int video_source = atoi(argv[2]);
		if (!capture.open(video_source)) {
			printf("Couldn't capture video!\n");
		}


	} else if (!strcasecmp(argv[1], "VIDEOFILE")) {
		// Play from a file
		printf("   Mode: File playback\n   File: %s\n\n", argv[2]);
		if (!capture.open(argv[2])) {
			printf("Invalid video file name!\n");
			exit(1);
		}

	} else if (!strcasecmp(argv[1], "IMAGEFILE")) {
		// Play from a file
		printf("   Mode: Image process\n   File: %s\n\n", argv[2]);
		image = imread(argv[2]);
		if (image.empty()) {
			printf("Invalid image file name!\n");
			exit(1);
		}
		cvtColor(image, image, CV_RGB2BGR);


	} else {
		// Invalid
		print_help();
		exit(1);
	}

	//Set Up LCM

	Comp *self = (Comp*) calloc (1, sizeof (Comp));
	  self->publish_lcm=lcm_create(NULL);
	  //self->subscribe_lcm = lcm_create(NULL);
	  self->counter =0;


	//printf("   Capturing at:\n      Width: %d\n      Height:%d\n", width, height);

	while(1) {
		Mat frame;
		if (image.empty()) {
			capture>>frame;
			if(frame.empty()) break;
		} else {
			frame = image;
		}

		bot_core_image_metadata_t fake;
		fake.n = 1;
		bot_image.width = frame.cols;
		bot_image.height = frame.rows;
		bot_image.row_stride = frame.channels()*frame.cols;
		bot_image.pixelformat = BOT_CORE_IMAGE_T_PIXEL_FORMAT_RGB;
		bot_image.data = frame.data;
		bot_image.utime = (long int) time(0);
		bot_image.size = frame.rows*frame.channels()*frame.cols;
		bot_image.nmetadata = 0;
		bot_image.metadata = &fake;


		// Send over the data!
		bot_core_image_t_publish(self->publish_lcm,lcmChannel1, &bot_image);

		if (image.empty()) {
			char c = cvWaitKey(1);
		} else {
			char c = cvWaitKey(500);
		}
	}


}
