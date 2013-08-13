/*
 * lcmconfig.hpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#ifndef LCMCONFIG_HPP_
#define LCMCONFIG_HPP_

#include "handler.hpp"

#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_core.hpp>
#include <blicycle/blicycle_packet_t.hpp>

typedef struct _BearingInformation{
	int lock;
	float rho;
	float theta;
} BearingInformation;

//maybe work on an LCM class that holds all this stuff and has getters and setters where needed
class LcmEngine{

	char* lcmChannelIn;
	char* lcmChannelOut;
	bool _good;


public:
	blicycle::blicycle_packet_t bikepacket;
	lcm::LCM globalLCM;
	Handler imageHandler;

	LcmEngine();
	~LcmEngine(){}

	void init(char*, char*);

	void subscribe();

	void handle();

	bool good();

	void publishLCM(int32_t, double, double);
};

#endif /* LCMCONFIG_HPP_ */
