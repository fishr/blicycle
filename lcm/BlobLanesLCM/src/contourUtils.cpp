/*
 * contourUtils.cpp
 *
 *  Created on: Aug 12, 2013
 *      Author: root
 */

#include "config.hpp"


#include "contourUtils.hpp"

#include <iterator>

std::vector<cv::Point> ContourUtils::contourMinMaxY(std::vector<cv::Point> &contour){
	cv::vector<cv::Point> minMax;
	minMax.push_back(contour[0]);
	minMax.push_back(contour[0]);
	for(unsigned i = 0; i<contour.size(); i++){
		if(contour.at(i).y > minMax.at(1).y){
			minMax[1] = contour.at(i);
		}else if(contour.at(i).y < minMax.at(0).y){
			minMax[0] = contour.at(i);
		}
	}
	return minMax;
}

std::vector<std::vector<cv::Point> > ContourUtils::splitSharpContours(std::vector<cv::Point> &contour, int width, int speed1, double maxAngle){
	const int backtrack = 5;
	int speed = speed1;
	if(speed>width)
		speed=width;
	std::vector<std::vector<cv::Point> > newContour;
	std::vector<int> splits;
	splits.push_back(0);
	bool done = false;
	bool interesting = false;
	int i = backtrack;
	while(!done){
		while(!interesting){
			if(i>=(((int)contour.size())-width-speed-backtrack)){
				done=true;
				break;
			}
			double angle = fabs(ContourUtils::angleDiff(contour.at(i), contour.at(i+1), contour.at(i+width), contour.at(i+width+1)));
			if(maxAngle<angle){
				interesting = true;
			}else{
				i=i+speed;
			}
		}

		if(interesting){
			if(i>=(contour.size()-width-speed-backtrack)){
				done=true;
				interesting = false;
				break;
			}
			int strikes = 0;
			for(int j=1; j<(backtrack+1); j++){
				double angle = fabs(ContourUtils::angleDiff(contour.at(i-j), contour.at(i+1-j), contour.at(i+width+j), contour.at(i+j+width+1)));
				if(maxAngle<angle)
					strikes++;
			}
			if(strikes>=3){
				splits.push_back(i+width/2);
			}
			interesting = false;
			i=speed+backtrack+i;
		}
	}

	if(splits.size()==1){
		newContour.push_back(contour);
		return newContour;
	}

	splits.push_back(contour.size()-1);
	for(unsigned i = 1; i<splits.size(); i++){  //TODO: switch to contour return trim if works
		std::vector<cv::Point>::iterator it1 = contour.begin();
		std::vector<cv::Point>::iterator it2 = contour.begin();
		std::advance(it1,splits.at(i-1));
		std::advance(it2,splits.at(i));
		std::vector<cv::Point> temp(it1, it2);
		newContour.push_back(temp);
	}
	return newContour;
}

void ContourUtils::reduceContourNodes(std::vector<cv::Point> &in, int length){
	for(int i=(in.size()-length-1); i>0; i-=length){
		if(3.1415/18>angleDiff(in.at(i), in.at(i+length/2), in.at(i+length/2), in.at(i+length))){
			std::vector<cv::Point>::iterator it1 = in.begin();
			std::vector<cv::Point>::iterator it2 = in.begin();
			std::advance(it1, i+1);
			std::advance(it2, i+length-1);
			in.erase(it1, it2);
		}
	}
}

void ContourUtils::processContours(std::vector<std::vector<cv::Point> > &in){
	eliminateSmalls(in, 10);
	std::vector<std::vector<cv::Point> > temp;
	for(unsigned i = 0; i<in.size(); i++){

		//reduceContourNodes(in.at(i), 12);

		std::vector<double> tempVec = contourOrientation(in.at(i));

		if(fabs(tempVec[0])>1 && tempVec[1]>-100){
			temp.push_back(in.at(i));
			printf("%f, %f\n", tempVec[0], tempVec[1]);
		}
	}
	in.swap(temp);
}

void ContourUtils::eliminateSmalls(std::vector<std::vector<cv::Point> > &in, float minRadius){
	std::vector<std::vector<cv::Point> > cleaned;
	for(unsigned i=0; i<in.size(); i++){
		float radius;
		cv::Point2f center;
		cv::minEnclosingCircle(in.at(i), center, radius);
		if(radius>minRadius){
			cleaned.push_back(in.at(i));
		}
	}
	in.swap(cleaned);
}

std::vector<double> ContourUtils::contourOrientation(std::vector<cv::Point> &in){
	double angle, ecc;
	double diff;
	cv::Moments data = cv::moments(in);
	diff = data.mu20 - data.mu02;
	if(data.mu11==0){
		if(diff<0){
			angle = -3.1415/2;
		}else{
			angle = 0;
		}
	}else if(diff==0){
		if(data.mu11>0){
			angle = 3.1415/4;
		}else{
			angle = -3.1415/4;
		}
	}else if(diff>0){
		if(data.mu11>0){
			angle = .5*(atan((2*data.mu11)/(diff)));
		}else{
			angle = -0.5*(atan((2*data.mu11)/(diff)));
		}
	}else{
		if(data.mu11>0){
			angle = .5*(atan((2*data.mu11)/(diff)))+3.1415/4;
		}else{
			angle = -0.5*(atan((2*data.mu11)/(diff)))-3.1415/4;
		}
	}
	ecc = (    ((diff*diff)-(4*data.mu11*data.mu11))   /   ((data.mu20+data.mu02)*(data.mu20+data.mu02))   );
	std::vector<double> result;
	result.push_back(angle);
	result.push_back(ecc);
	return result;
}

std::vector<cv::Point> ContourUtils::contourReturnTrim(int start, int end, std::vector<cv::Point> &in){
	std::vector<cv::Point>::iterator it1 = in.begin();
	std::vector<cv::Point>::iterator it2 = in.begin();
	std::advance(it1, start);
	std::advance(it2, end);
	return std::vector<cv::Point>(it1, it2);
}

double ContourUtils::angleDiff(std::vector<cv::Point> &a, std::vector<cv::Point> &b){
	double angle1 = atan2(a[0].y-a[1].y, a[0].x-a[1].x);
	double angle2 = atan2(b[0].y-b[1].y, b[0].x-b[1].x);
	return angle1-angle2;
}

double ContourUtils::angleDiff(std::vector<cv::Point> a, std::vector<cv::Point> b){
	double angle1 = atan2(a[0].y-a[1].y, a[0].x-a[1].x);
	double angle2 = atan2(b[0].y-b[1].y, b[0].x-b[1].x);
	return angle1-angle2;
}

double ContourUtils::angleDiff(cv::Point a0, cv::Point a1, cv::Point b0, cv::Point b1){
	double angle1 = atan2(a0.y-a1.y, a0.x-a1.x);
	double angle2 = atan2(b0.y-b1.y, b0.x-b1.x);
	return angle1-angle2;
}
