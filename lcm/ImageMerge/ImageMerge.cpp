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

int msgHeight = 0;
int msgWidth = 0;


Mat iOne;
Mat iTwo;

bot_core_image_t bot_image;



void on_image_frame1(const lcm_recv_buf_t *rbuf, const char *channel,const bot_core_image_t *msg, void *user_data) {
	Comp *self = (Comp*) user_data;


	Mat img_lcm = Mat::zeros(msg->height, msg->width, CV_8UC3); // h,w
	img_lcm.data = msg->data;

	img_lcm.copyTo(iOne);//probs shouldnt copy  TODO
	//self->iOne = img_lcm;
	gotOne = true;

	msgHeight = msg->height;
	msgWidth = msg->width;

	//printf("one!\n");

}

void on_image_frame2(const lcm_recv_buf_t *rbuf, const char *channel, const bot_core_image_t *msg, void *user_data) {
	Comp *self = (Comp*) user_data;


	Mat img_lcm = Mat::zeros(msg->height, msg->width, CV_8UC3); // h,w
	img_lcm.data = msg->data;

	img_lcm.copyTo(iTwo);
	//self->iTwo = img_lcm;
	gotTwo = true;

	//printf("two!\n");

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
			IplImage* stitch = cvCreateImage(cvSize(msgWidth*2, msgHeight), IPL_DEPTH_8U, 3);

			cvSetImageROI(stitch, cvRect(0,0, msgWidth, msgHeight));
			IplImage* iOnea = new IplImage(iOne);  //maybe I do have to copy but this seems unecessary TODO
			cvCopy(iOnea, stitch, 0);

			cvSetImageROI(stitch, cvRect(msgWidth,0, msgWidth, msgHeight));
			IplImage* iTwoa = new IplImage(iTwo);
			cvCopy(iTwoa, stitch, 0);

			cvResetImageROI(stitch);

			cvCvtColor(stitch, stitch, CV_BGR2RGB);
			//cvShowImage("testing", stitch);

			cvWaitKey(1);

			bot_core_image_metadata_t fake;
			fake.n = 1;
			bot_image.width = stitch->width;
			bot_image.height = stitch->height;
			bot_image.row_stride = stitch->nChannels*stitch->width;
			bot_image.pixelformat = BOT_CORE_IMAGE_T_PIXEL_FORMAT_BGR;
			bot_image.data = (uchar*)stitch->imageData;
			bot_image.utime = (long int) time(0);
			bot_image.size = stitch->imageSize;
			bot_image.nmetadata = 0;
			bot_image.metadata = &fake;

			bot_core_image_t_publish(self->publish_lcm, lcmChannel3, &bot_image);


			//cvReleaseImage(&iOnea);
			//cvReleaseImage(&iTwoa);
			cvReleaseImage(&stitch);

			gotOne = gotTwo = false;
		}

	}
}

