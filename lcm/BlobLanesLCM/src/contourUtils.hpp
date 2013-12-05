/*
 * contourUtils.hpp
 *
 *  Created on: Aug 12, 2013
 *      Author: root
 */

#ifndef CONTOURUTILS_HPP_
#define CONTOURUTILS_HPP_

class ContourUtils{
public:

	static std::vector<cv::Point> contourMinMaxY(std::vector<cv::Point>&);

	static std::vector<std::vector<cv::Point> > splitSharpContours(std::vector<cv::Point>&, int, int, double);

	static void reduceContourNodes(std::vector<cv::Point>&, int);

	static std::vector<cv::Point> contourReturnTrim(int, int, std::vector<cv::Point>&);

	static void processContours(std::vector<std::vector<cv::Point> >&);

	static void eliminateSmalls(std::vector<std::vector<cv::Point> >&, float);

	static std::vector<double> contourOrientation(std::vector<cv::Point>&);

	static double angleDiff(std::vector<cv::Point>&, std::vector<cv::Point>&);

	static double angleDiff(std::vector<cv::Point>, std::vector<cv::Point>);

	static double angleDiff(cv::Point, cv::Point, cv::Point, cv::Point);
};

#endif /* CONTOURUTILS_HPP_ */
