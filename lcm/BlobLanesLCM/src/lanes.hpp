/*
 * lanes.hpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */



#ifndef LANES_HPP_
#define LANES_HPP_

#include "opencv2/opencv.hpp"

class Line{ //a class for final lines, which we will compare the current frames identified lines against to merge and update
public:
	float confidence;
	cv::Vec4i endpoints;
};

class Lane : public Line {
public:
	int id;  //counting from the inside
};

class Track{

public:
	std::vector<Lane> lanes[10];
};

#endif /* LANES_HPP_ */
