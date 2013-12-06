/*
 * vectorhelp.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */

#include "config.hpp"

#include "vectorhelp.hpp"

using namespace cv;

vector<int> stripVectorRow(vector<Vec3i> fullVector, int row){
	vector<int> output;
	output.resize(fullVector.size());
	for(unsigned int i =0; i< fullVector.size(); i++){
		output[i]=fullVector[row][i];
	}
	return output;
}

int nearestElementIndex(vector<int> searchThis, int findThis){
	int bestFit = 0;
	for(unsigned int i =0; i<searchThis.size(); i++){
		if(abs(searchThis[i]-findThis)<abs(searchThis[bestFit]-findThis)){
			bestFit = i;
		}
	}
	return bestFit;
}

vector<float> stripVectorRow(vector<Vec3f> fullVector, int row){
	vector<float> output;
	output.resize(fullVector.size());
	for(unsigned int i =0; i< fullVector.size(); i++){
		output[i]=fullVector[row][i];
	}
	return output;
}

int nearestElementIndex(vector<float> searchThis, float findThis){
	int bestFit = 0;
	for(unsigned int i =0; i<searchThis.size(); i++){
		if(abs(searchThis[i]-findThis)<abs(searchThis[bestFit]-findThis)){
			bestFit = i;
		}
	}
	return bestFit;
}

/*void reduceVec3i(vector<Vec3i> bigVector){
	for(int i=0; i<bigVector; i++){
		if(bigVector[i][0]!=0)
	}
}
*/


