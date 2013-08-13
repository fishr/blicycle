/*
 * vectorhelp.hpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#ifndef VECTORHELP_HPP_
#define VECTORHELP_HPP_

#include "opencv2/opencv.hpp"

void reduceVector(cv::vector<int>&);

cv::vector<int> stripVectorRow(cv::vector<cv::Vec3i>, int);

int nearestElementIndex(cv::vector<int>, int);

cv::vector<float> stripVectorRow(cv::vector<cv::Vec3f>, int);

int nearestElementIndex(cv::vector<float>, float);

#endif /* VECTORHELP_HPP_ */
