/*
 * handler.hpp
 *
 *  Created on: Aug 8, 2013
 *      Author: root
 */

#ifndef HANDLER_HPP_
#define HANDLER_HPP_

#include "vectorhelp.hpp"

#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_core.hpp>
#include <blicycle/blicycle_packet_t.hpp>

class Handler{
	bool _flag;

public:
	int width;
	int height;

	cv::Mat src;

	~Handler(){}

	Handler();

	void handleMessage(const lcm::ReceiveBuffer*, const std::string&, const bot_core::image_t*);

	bool newImage();

};


#endif /* HANDLER_HPP_ */
