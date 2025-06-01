/*
* 这是一个图像预处理类，直接使用OpenCV库的函数一会这一会那的有点烦，
* 为了方便就写了这玩意。
* 
* 写这条注释主要怕我自己忘了怎么用。
* 
* 顺便提醒一下自己：不要在头文件使用using namespace，免得污染全局命名空间。
* 
* 你可以直接使用preprocess_image函数来处理图像
* 目前包括：
* 1. 缩放图像
* 2. 灰度图像
* 3. 二值图像
* 4. HSV过滤图像
* 
* 使用时只需提供图片路径和所需的处理类型即可。
* 注意：
* 1.缩放图像和灰度图像是基于原始图像的，二值图像是基于灰度图像的。
* 2.HSV过滤需要提供HSV范围的向量，这个向量中的HSVRangers结构体定义在MyTypes.h中。
*/
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <vector>
#include "MyTypes.h"

#pragma once
class ImgProcessor {
private:

	std::string imgPath; // 图片路径
	cv::Mat originalImg; // 原始图像
	int size_x, size_y; // 图像尺寸

public:

	static enum IMG_TYPE {
		RESIZED_IMG = 0, // 缩放后的图像
		GRAY_IMG,       // 灰度图像
		BINARY_IMG,        // 二值图像
		HSV_FILTERED_IMG // HSV过滤后的图像
	};

	ImgProcessor(const std::string path, const int size_x, const int size_y) : imgPath(path), size_x(size_x), size_y(size_y){
		originalImg = cv::imread(imgPath); // 读取图片
	}
	~ImgProcessor() {}

	cv::Mat preprocess_image(IMG_TYPE type, std::vector<HSVRanges> hsvRanges) {
		if (originalImg.empty()) {
			std::cout << "图片加载失败！" << std::endl;
			return cv::Mat(); // 返回空矩阵
		}
		switch (type) {
		case HSV_FILTERED_IMG: {
			cv::Mat hsvFilteredImg = originalImg.clone();
			for (const auto& range : hsvRanges) {
				cv::inRange(originalImg, range.lower, range.upper, hsvFilteredImg); // 应用HSV范围过滤
			}
			return hsvFilteredImg;
		}
		default:
			std::cout << "未知类型！" << std::endl; 
			return cv::Mat(); // 返回空矩阵
		}
	}

	cv::Mat preprocess_image(IMG_TYPE type) {
		cv::Mat img = cv::imread(imgPath); // 读取图片
		if (img.empty()) {
			std::cout << "图片加载失败！" << std::endl;
			return cv::Mat();
		}
		switch (type) {
		case RESIZED_IMG: {
			cv::Mat resizedImg;
			cv::resize(originalImg, resizedImg, cv::Size(size_x, size_y)); // 缩放图像
			return resizedImg;
		}
		case GRAY_IMG: {
			cv::Mat grayImg;
			cv::Mat resizedImg = preprocess_image(RESIZED_IMG); // 先缩放图像
			cv::cvtColor(resizedImg, grayImg, cv::COLOR_BGR2GRAY); // 转换为灰度图像
			return grayImg;
		}
		case BINARY_IMG: {
			cv::Mat grayImg = preprocess_image(GRAY_IMG);
			cv::Mat binaryImg;
			cv::threshold(grayImg, binaryImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // Otsu's 二值化
			return binaryImg;
		}
		default:
			std::cout << "未知类型！" << std::endl;
			return cv::Mat(); // 返回空矩阵
		}
	}

	cv::Mat preprocess_image() {
		return preprocess_image(RESIZED_IMG); // 默认返回缩放后的图像
	}
};

