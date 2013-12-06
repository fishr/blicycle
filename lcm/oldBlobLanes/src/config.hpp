/*
 * config.hpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "opencv2/opencv.hpp"

#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_core.hpp>
#include <blicycle/blicycle_packet_t.hpp>

#include <stdio.h>

 extern double cameraMatData[9];
 extern cv::Mat cameraMatrix;

 extern double distortionMatData[8];
 extern cv::Mat distMatrix;


  // Print help information
  void print_help();

#endif /* CONFIG_HPP_ */
