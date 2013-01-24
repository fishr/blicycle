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

using namespace std;
using namespace cv;

bool gotOne = false;
bool gotTwo = false;

typedef struct _Comp {
	lcm_t* subscribe_lcm;
	lcm_t* publish_lcm;
}Comp;

int msg1Height = 0;
int msg1Width = 0;

int msg2Height = 0;
int msg2Width = 0;

Mat iOne;
Mat iTwo;

bot_core_image_t bot_image;



void on_image_frame1(const lcm_recv_buf_t *rbuf, const char *channel,const bot_core_image_t *msg, void *user_data) {
	Comp *self = (Comp*) user_data;


	Mat img_lcm = Mat::zeros(msg->height, msg->width, CV_8UC3); // h,w
	img_lcm.data = msg->data;
	img_lcm.copyTo(iOne);
	//self->iOne = img_lcm;
	gotOne = true;

	msg1Height = msg->height;
	msg1Width = msg->width;

	//printf("one!\n");

}

void on_image_frame2(const lcm_recv_buf_t *rbuf, const char *channel, const bot_core_image_t *msg, void *user_data) {
	Comp *self = (Comp*) user_data;


	Mat img_lcm = Mat::zeros(msg->height, msg->width, CV_8UC3); // h,w
	img_lcm.data = msg->data;
;
	img_lcm.copyTo(iTwo);
	//self->iTwo = img_lcm;
	gotTwo = true;

	//printf("two!\n");

	msg2Height = msg->height;
	msg2Width = msg->width;

}

// Print help information
void print_help() {
	printf("** Invalid syntax!\n"
			"   Usage: ImageMerge <input1> <input2> <output>\n"
			"   LCM channels only\n");
}

int main(int argc, char** argv) {

	printf("ImageMerge\n");

	if (argc != 4){
		print_help();
		exit(1);
	}


	char* lcmChannel1 = argv[1];
	char* lcmChannel2 = argv[2];
	char* lcmChannel3 = argv[3];

	Comp *self = (Comp*) calloc(1, sizeof(Comp));

	//cvZero(&(self->iOne));
	//cvZero(&(self->iTwo));

	self->publish_lcm = lcm_create(NULL);
	self->subscribe_lcm = lcm_create(NULL);


	bot_core_image_t_subscription_t * sub1 = bot_core_image_t_subscribe(self->subscribe_lcm, lcmChannel1, on_image_frame1, self);
	bot_core_image_t_subscription_t * sub2 = bot_core_image_t_subscribe(self->subscribe_lcm, lcmChannel2, on_image_frame2, self);

	//cvNamedWindow("testing", CV_WINDOW_AUTOSIZE);

	while(1){
		lcm_handle(self->subscribe_lcm);

		if (gotOne&&gotTwo){
			//printf("running stitch\n");
			Mat stitch = Mat::zeros(max(msg1Height,msg2Height), msg1Width+msg2Width, CV_8UC3);


			iOne.copyTo(stitch(Rect(0,0, msg1Width, msg1Height)));
			iTwo.copyTo(stitch(Rect(msg1Width,0, msg2Width, msg2Height)));

			//imshow("testing", stitch);

			cvWaitKey(1);

			bot_core_image_metadata_t fake;
			fake.n = 1;
			bot_image.width = stitch.cols;
			bot_image.height = stitch.rows;
			bot_image.row_stride = stitch.channels()*stitch.cols;
			bot_image.pixelformat = BOT_CORE_IMAGE_T_PIXEL_FORMAT_RGB;
			bot_image.data = stitch.data;
			bot_image.utime = (long int) time(0);
			bot_image.size = stitch.total()*stitch.channels();
			bot_image.nmetadata = 0;
			bot_image.metadata = &fake;

			bot_core_image_t_publish(self->publish_lcm, lcmChannel3, &bot_image);


			stitch.~Mat();

			gotOne = gotTwo = false;
		}

	}
	iOne.~Mat();
	iTwo.~Mat();
}

