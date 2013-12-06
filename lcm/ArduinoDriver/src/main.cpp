/*
 * main.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: Ryan Fish
 */


#include <SerialStream.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_param/entry_t.hpp>
#include <thread>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT "/dev/ttyUSB0" //This is needs checking
#define CHANNEL "to_arduino"
const int minBattVolts = 600; //this needs checking
const int arduinoTimeout = 1000;
const double speedConversion = 184.0;  //based on cold hard testing

lcm::LCM lcmPod;

bot_param::entry_t steeringMsg;
bot_param::entry_t velocityMsg;
bot_param::entry_t batteryMsg;
bot_param::entry_t leftButtonMsg;
bot_param::entry_t rightButtonMsg;


int cmd = -1;

int stringtoint(std::string);
std::string inttostring(int);

class Handler {
public:
	~Handler() {};

	void onMessage(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const bot_param::entry_t* msg) {
		cmd = stringtoint(msg->value);
		//int tmp = stringtoint(msg->value);
		//if(tmp!=cmd)
		//	cmd = tmp;
	}
};

int steering;
int battery;
int speed100;
bool leftButton;
bool rightButton;
bool leftButtonFlag = false;
bool rightButtonFlag = false;

LibSerial::SerialStream ardu;
SerialPort* arduino;

void openArduino(){
	arduino = new SerialPort(PORT);
	if(!arduino->IsOpen()){
		try{
			arduino->Open(SerialPort::BAUD_115200);
			printf("Arduino ready!\n");
		}
		catch(SerialPort::OpenFailed&){
			printf("couldn't open port\n");
			sleep(1);
			openArduino();
		}
	}


	/*
    //The arduino must be setup to use the same baud rate
    ardu.Open(PORT);
    ardu.SetBaudRate(LibSerial::SerialStreamBuf::BAUD_115200);
    if(ardu.IsOpen())
    	return;

    printf("Couldn't connect!");
    exit(1);
    */
}


void thread1(int n){
	while(true)
		lcmPod.handle();
}

void openLCM(Handler& handle){
	if(lcmPod.good()){
		printf("LCM good to go\n");
	}else{
		printf("LCM broken\n");
		exit(1);
	}

	lcmPod.subscribe(CHANNEL, &Handler::onMessage, &handle);
	printf("subscribed to ");
	printf(CHANNEL);
	printf("\n");

	std::thread t1(thread1, 1);  //not actually an issue, not sure whats up here
	t1.detach();
}


std::string inttostring(int num){
	std::stringstream ss;
	ss<<num;
	return ss.str();
}

void sendLCMUpdates(){
	steeringMsg.value = inttostring(steering);
	lcmPod.publish<bot_param::entry_t>("arduinoUpdates", &steeringMsg);
	//lcmPod.publish("steeringMsg", &steering, sizeof(steering));

	//only need to post about battery if its a warning
	if(battery<minBattVolts){
		batteryMsg.value = inttostring(battery);
		lcmPod.publish<bot_param::entry_t>("arduinoUpdates", &batteryMsg);
	}

	velocityMsg.value = inttostring(speed100);
	lcmPod.publish<bot_param::entry_t>("arduinoUpdates", &velocityMsg);

	//maybe these should only send if they change as well
	if(leftButtonFlag){
		leftButtonMsg.value = inttostring(leftButton);
		lcmPod.publish<bot_param::entry_t>("arduinoUpdates", &leftButtonMsg);
		leftButtonFlag = false;
	}
	if(rightButtonFlag){
		rightButtonMsg.value = inttostring(rightButton);
		lcmPod.publish<bot_param::entry_t>("arduinoUpdates", &rightButtonMsg);
		rightButtonFlag = false;
	}
}


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int stringtoint(std::string in){
	return atoi(in.c_str());
}

void parseInput(std::string in){
	std::vector<std::string> values;
	std::stringstream ss(in);

	//check if we have a full message
	int commas = std::count(in.begin(), in.end(), ',');
	if(commas == 4){
		values.clear();
		split(in, ',', values);
	}else if(in.find("help",0)!=std::string::npos){
		//arduino is blocked
		//TODO: should report this
		printf("arduino is in trouble!\n");
		return;
	}
	else{
		printf("bad message\n");
		return;
	}
	std::vector<int> value;
	for(int i = 0; i<5; i++){
		value.push_back(stringtoint(values.at(i)));
	}

	steering = value.at(0);
	battery  = value.at(1);
	int speedRaw = (int) value.at(2);
	if(speedRaw != 0){
		speed100 = (int) (speedConversion*100)/((int) value.at(2));
	}else{
		speed100 = 0;
	}
	if(leftButton!=value.at(3)){
		leftButton = value.at(3);
		leftButtonFlag = true;
	}
	if(rightButton!=value.at(4)){
		rightButton = value.at(4);
		rightButtonFlag = true;
	}
}

void packUpdates(){
	steeringMsg.key = "s";
	batteryMsg.key = "b";
	velocityMsg.key = "v";
	leftButtonMsg.key = "l";
	rightButtonMsg.key = "r";
}

void sendArduinoUpdates(){
	if(arduino->IsOpen())
		try{
			arduino->WriteByte((unsigned char)((char)cmd));
		}
	catch(std::runtime_error&){
		std::cout<<"boooo runtime\n";
	}
	printf(inttostring(cmd).c_str());
	printf("\n");
}

std::string readArduino(int timeout){
	if(arduino->IsOpen()){
	try{
				return arduino->ReadLine(2000);
			}
			catch(SerialPort::ReadTimeout&){
				printf("no new msg\n");
				//TODO: this should send a warning to other warning processes that somethings up
				//and check if arduino still exists somehow
			}
			catch(SerialPort::NotOpen&){
				printf("NO ARDUINO @  ");
				printf(PORT);
				printf(" -- trying to get connection back\n");
				openArduino();
			}
			catch(std::runtime_error&){
				std::cout<<"bad runtime, bad\n";
			}
			catch(...){
				printf("Ouch, something bad happened\n");
				exit(1);
			}
	}
			return "";
}

void detachComms(){
	arduino->Close();
	lcmPod.~LCM();
	puts("comms closed\n");
}



int main( int argc, char** argv ) {
	openArduino();

	Handler handler;
	openLCM(handler);

	packUpdates();

	atexit(detachComms);

	while(1){
		std::string msgIn = readArduino(arduinoTimeout);
		//printf(msgIn.c_str());
		//printf("\n");
		if(!msgIn.empty()){
			parseInput(msgIn);
			sendLCMUpdates();
			sendArduinoUpdates();
		}
	}
}

