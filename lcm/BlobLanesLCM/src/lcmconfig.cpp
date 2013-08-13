/*
 * lcmconfig.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#include "config.hpp"

#include "lcmconfig.hpp"

LcmEngine::LcmEngine(){
}

void LcmEngine::init(char* in, char* out){
	lcmChannelIn = in;
	lcmChannelOut = out;

	_good = globalLCM.good();
}

bool LcmEngine::good(){
	return _good;
}

void LcmEngine::handle(){
	globalLCM.handle();
}

void LcmEngine::subscribe(){
	globalLCM.subscribe(lcmChannelIn, &Handler::handleMessage, &imageHandler);
}

/**
 * A helper method to publish data over a socket.
 */
void LcmEngine::publishLCM(int32_t lock, double rho, double theta) {

	bikepacket.timestamp = (int64_t)time(NULL);
	bikepacket.lock=lock;   //effectively a boolean if current lane is know, may actually return some relevant value as well, but zero if unsure
	bikepacket.phi = theta;    //rotational error
	bikepacket.lane = 3;    //current lane
	bikepacket.totalLanes = 5; //total lanes in view
	bikepacket.delta = 0;  //lateral error from center of lane

	globalLCM.publish(lcmChannelOut, &bikepacket);
}



