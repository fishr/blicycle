/*
 * handler.cpp
 *
 *  Created on: Aug 8, 2013
 *      Author: root
 */

#include "config.hpp"

#include "handler.hpp"

/*
class Handler{

	cv::Mat srcCopy, blurred, denoised_img, src_gray;

public:
	~Handler() {}

	Handler(){
		src_gray(cv::Size(inX,inY), CV_32FC1, cv::Scalar(0));
	}

	void handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const bot_core::image_t* msg) {

		cv::Mat src(msg->height, msg->width, CV_8UC3);
		src.data = (uchar*)&msg->data[0];

		if(!calibrated)
			calibrateCameraConsts(msg->width, msg->height);
		calibrated = true;

		cv::imshow("raw", src);

		cv::remap(src, srcCopy, map1, map2, cv::INTER_LINEAR);
		imshow("test", srcCopy);

		/// Pass the image to gray
		//cvtColor( srcCopy, src_gray, CV_RGB2GRAY );
		cvtColor(srcCopy,srcCopy,CV_RGB2GRAY);

		cv::resize(srcCopy,srcCopy, cv::Size(inX, inY));

		//downsample the image
		//pyrDown(src_gray, blurred);
		//imshow("blurred", blurred);

		//add to running average image to help blur and smooth
		accumulateWeighted(srcCopy, src_gray, accumAlpha/100.0);
		src_gray.convertTo(blurred,CV_8UC1);

		blurred.adjustROI(-cropHeight, -cropLow, 0, 0);
		xsize=blurred.cols;
		ysize=blurred.rows;

		blacknwhite(0,0);

		lowHeight = denoised_img.rows-10;
		midHeight = denoised_img.rows/3;
		highHeight = min(40, denoised_img.rows/4);

		updateLines(0,0);
		cv::waitKey(1);
	}

};
 */

Handler::Handler(){
	_flag = false;
}

void Handler::handleMessage(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const bot_core::image_t* msg) {
	cv::Mat src1= cv::Mat::zeros(msg->height, msg->width, CV_8UC3);
	src1.data = (uchar*)&msg->data[0];
	_flag = true;
	height = msg->height;
	width = msg->width;
	src1.copyTo(src);
}

bool Handler::newImage(){
	bool tmp = _flag;
	_flag = false;
	return tmp;
}
