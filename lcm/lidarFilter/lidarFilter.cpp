/*
 * lidarFilter.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: blicycle
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "lcm/lcm.h"
#include "lcmtypes/bot_core.h"
#include <ctime>


using namespace std;
using namespace cv;
typedef struct _Comp {
  lcm_t* subscribe_lcm;
  lcm_t* publish_lcm;
}Comp;

Mat lidar;

int imageHeight = 10;

// Print help information
void print_help() {
    printf("** Invalid syntax!\n"
    		"<channelIn> <channelOut>\n");
}


void on_frame(const lcm_recv_buf_t *rbuf, const char *channel,
		const bot_core_planar_lidar_t *msg, void *user_data){
	Comp *self = (Comp*) user_data;

	lidar.create(imageHeight+1, msg->nranges, CV_32FC1);
	for(int i = 0; i<msg->nranges; i++){
		float intensity = msg->ranges[i]/8.0;
		for(int k = 0; k<imageHeight; k++){
			lidar.at<float>(k,i)= intensity;
		}
	}


	//printf("%f\n", msg->ranges[msg->nranges/2]);
}


int main( int argc, char** argv ) {

	// Print a welcome message
	printf("lidar Filter\n");

	if (argc != 3) {
		print_help();
		exit(1);
	}

	char* lcmChannel1 = argv[1];
	char* lcmChannel2 = argv[2];

	//Set Up LCM

	Comp *self = (Comp*) calloc (1, sizeof (Comp));
	  self->publish_lcm=lcm_create(NULL);
	  self->subscribe_lcm = lcm_create(NULL);

	  bot_core_planar_lidar_t_subscription_t * sub = bot_core_planar_lidar_t_subscribe(self->subscribe_lcm, lcmChannel1, on_frame, self);

	  cvNamedWindow("testing");

	while(1) {
		lcm_handle(self->subscribe_lcm);

		imshow("testing", lidar);

		cvWaitKey(3);
	}
	lidar.~Mat();
	cvDestroyWindow("testing");
}
