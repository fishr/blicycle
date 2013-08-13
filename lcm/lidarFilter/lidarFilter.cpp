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
#include "lcm/lcm-cpp.hpp"
#include "lcmtypes/bot_core.hpp"
#include "lcmtypes/bot_param/entry_t.hpp"
#include <ctime>
#include <list>
#include <thread>
#include <boost/thread.hpp>


using namespace std;
using namespace cv;

lcm::LCM lcmPod;

Mat lidar;

int imageHeight = 10;
const float obstacleThresh = 0.5;  //half a meter difference will trigger an obstacle warning for now
const float criticalThresh = .5;  //5 meters away, an obstacle will trigger the stop command
bool critical = false;
const float imageWhiteout = 8.0;
const int imageChannels = 3;
boost::mutex imageLock;

bot_param::entry_t arduinoMsg;

char* lcmChannel1;
char* lcmChannel2;
char* lcmChannel3;

void sendArduinoMsg(int input);
void sendAlarm();
void sendSilencer();

class Handler {
public:
	~Handler() {};

	void onMessage(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const bot_core::planar_lidar_t* msg){
		boost::unique_lock<boost::mutex> lock(imageLock, boost::try_to_lock);
			if(lock.owns_lock()){
				int critCount = 0;
				std::list<int> warningZones;
				int colorOffset = 0;
				lidar.create(Size(msg->nranges, imageHeight+1), CV_32FC3);
				float currRange;
				for(int j = 10; j<msg->nranges-10; j++){
					int i = msg->nranges - j;
					currRange = msg->ranges[i];
					std::vector<float> medianOld = {msg->ranges[i-1],msg->ranges[i-2],msg->ranges[i-3]};
					std::sort(medianOld.begin(), medianOld.end());
					float diff;
					if(currRange<0.002){
						colorOffset = 3;
						currRange = 30;
						diff = 30-medianOld[1];
					}else{
						std::vector<float> medianNew = {msg->ranges[i],msg->ranges[i+1],msg->ranges[i+2]};
						std::sort(medianNew.begin(), medianNew.end());
						diff = medianNew[1]-medianOld[1];
					}
					if(currRange>29.){
						colorOffset = 3;
					}else if((currRange<criticalThresh)){
						critCount++;
						colorOffset = 2;
					}else if(diff>obstacleThresh){
						warningZones.push_back(i);
						colorOffset = 1;
					}else if(diff<((-1.0)*obstacleThresh)){
						warningZones.push_back(i);
						colorOffset = 3;
					}else{
						//colorOffset = 3;
					}
					for(int k = 0; k<imageHeight; k++){
						switch(colorOffset){
						case 0:
							lidar.at<Vec3f>(k,j) = {1, 0.0, 1};
							break;
						case 1:
							lidar.at<Vec3f>(k,j) = {0.0, 1, 1};
							break;
						case 2:
							lidar.at<Vec3f>(k,j) = {0.0, 0.0, 1};
							break;
						case 3:
							lidar.at<Vec3f>(k,j) = {0.0, 0.0, 0.0};
							break;
						}
					}
				}
				if(critCount>0)
					sendAlarm();
				if((critCount==0)&&critical){
					sendSilencer();
					critical = false;
				}
			}	};
};


// Print help information
void print_help() {
    printf("** Invalid syntax!\n"
    		"<channelIn> <channelOut>\n");
}

std::string inttostring(int num){
	std::stringstream ss;
	ss<<num;
	return ss.str();
}

void sendAlarm(){
	sendArduinoMsg(17);
}

void sendArduinoMsg(int input){
	arduinoMsg.value = inttostring(input);
	lcmPod.publish<bot_param::entry_t>(lcmChannel2, &arduinoMsg);
}

void sendSilencer(){
	sendArduinoMsg(255);
}

void displayImage(){
	boost::unique_lock<boost::mutex> lock(imageLock);
	imshow("testing", lidar);
}


int main( int argc, char** argv ) {

	// Print a welcome message
	printf("lidar Filter\n");

	if (argc != 4) {
		print_help();
		exit(1);
	}

	lcmChannel1 = argv[1];  //Lidar
	lcmChannel2 = argv[2];  //arduino
	lcmChannel3 = argv[3];  //controller

	//Set Up LCM
	if(!lcmPod.good()){
		printf("LCM not good\n");
		return 0;
	}
	Handler lidarMessage;
	lcmPod.subscribe(lcmChannel1, &Handler::onMessage, &lidarMessage);

	arduinoMsg.key = "alert";

	cvNamedWindow("testing");

	while(1) {
		lcmPod.handle();
		displayImage();
		cvWaitKey(3);
	}
	lidar.~Mat();
	cvDestroyWindow("testing");
}
