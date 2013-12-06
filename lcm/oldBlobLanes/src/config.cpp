/*
 * config.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#include "config.hpp"

double cameraMatData[9] = {576.61515089373336, 0., 375.50000000000000, 0., 576.61515089373336, 239.50000000000000, 0., 0., 1.};
cv::Mat cameraMatrix = cv::Mat(cv::Size(3,3), CV_64FC1, &cameraMatData);

double distortionMatData[8] = {-0.34642734082442700, 0.20169931123290941, 0., 0., -0.093070794789970671, 0., 0., 0.};
cv::Mat distMatrix = cv::Mat(cv::Size(1,8), CV_64FC1, &distortionMatData);

  // Print help information
  void print_help() {
  	printf("** Invalid syntax!\n"
  			"   Usage: blicycle  <source> <output>\n"
  			"       source: the LCM channel to use\n"
  			"		output: the LCM channel to use\n");
  }
