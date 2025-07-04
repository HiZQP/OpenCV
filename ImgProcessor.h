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
	int HSVDynamic(cv::Mat img);

public:

	bool HSVDynamicFlag = false; // 是否使用HSV动态过滤
	int size_x, size_y; // 图像尺寸
	cv::Mat originalImg; // 视频帧

	static enum IMG_TYPE {
		RESIZED_IMG = 0, // 缩放后的图像
		GRAY_IMG,       // 灰度图像
		BINARY_IMG,        // 二值图像
		HSV_FILTERED_IMG, // HSV过滤后的图像
		HSV_FILTERED_BINARY_IMG, // HSV过滤后的二值图像
		HSV_DYNAMIC_FILTERED_IMG, // HSV动态过滤后的图像
		HSV_DYNAMIC_FILTERED_BINARY_IMG // HSV动态过滤后的二值图像
	};
	
	ImgProcessor(const std::string path, const int size_x, const int size_y) : imgPath(path), size_x(size_x), size_y(size_y){
		cv::imread(path, originalImg); // 读取图片
	}
	
	ImgProcessor(const cv::Mat frame, const int size_x, const int size_y) : originalImg(frame), size_x(size_x), size_y(size_y) {
		
	}
	~ImgProcessor() {}

	cv::Mat preprocess_image(IMG_TYPE type, std::vector<HSVRanges> hsvRanges) {
		if (originalImg.empty()) {
			std::cout << "图片加载失败！" << std::endl;
			return cv::Mat(); // 返回空矩阵
		}
		switch (type) {
		case HSV_FILTERED_IMG: {
			cv::Mat HSVImg;
			cv::Mat resizedImg = preprocess_image(RESIZED_IMG); // 先缩放图像
			cvtColor(resizedImg, HSVImg, cv::COLOR_BGR2HSV);//转换为HSV色彩空间
			cv::Mat range_all; // 用于存储合并后的掩码
			std::vector<cv::Mat> masks; // 创建一个掩码向量，大小与HSV范围数量相同
			for (const auto& range : hsvRanges) {
				cv::Mat mask;
				cv::inRange(HSVImg, range.lower, range.upper, mask); // 创建掩码
				masks.push_back(mask); // 将掩码添加到向量中
			}
			for (size_t i = 0; i < masks.size(); i++) {
				if (i == 0) {
					range_all = masks[i]; // 初始化合并掩码
				}
				else {
					cv::bitwise_or(range_all, masks[i], range_all); // 合并掩码
				}
			}
			cv::Mat filteredImg;
			cv::bitwise_and(resizedImg, resizedImg, filteredImg, range_all); // 应用掩码提取颜色区域
			cv::cvtColor(filteredImg, filteredImg, cv::COLOR_BGR2GRAY); // 转换为灰度图像
			return filteredImg; // 返回过滤后的图像
		}
		case HSV_FILTERED_BINARY_IMG: {
			cv::Mat filteredImg = preprocess_image(HSV_FILTERED_IMG, hsvRanges); // 先进行HSV过滤
			cv::Mat binaryImg;
			cv::threshold(filteredImg, binaryImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // Otsu's 二值化
			return binaryImg; // 返回二值图像
		}
		case HSV_DYNAMIC_FILTERED_IMG: {
			cv::Mat filteredImg = preprocess_image(HSV_FILTERED_IMG, hsvRanges); // 先进行HSV过滤
			int Vlower = HSVDynamic(filteredImg); // 获得HSV亮度下限
			std::vector<HSVRanges> hsvRangesCopy = hsvRanges; // 复制HSV范围
			for (auto& range : hsvRangesCopy) {
				range.lower[2] = Vlower; // 更新亮度下限
			}
			return preprocess_image(HSV_FILTERED_IMG, hsvRangesCopy); // 重新进行HSV过滤
		}
		case HSV_DYNAMIC_FILTERED_BINARY_IMG: {
			cv::Mat filteredImg = preprocess_image(HSV_DYNAMIC_FILTERED_IMG, hsvRanges); // 先进行HSV动态过滤
			cv::Mat binaryImg;
			cv::threshold(filteredImg, binaryImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // Otsu's 二值化
			return binaryImg; // 返回二值图像
									 }
		default:
			std::cout << "未知类型！" << std::endl; 
			return cv::Mat(); // 返回空矩阵
		}
	}

	cv::Mat preprocess_image(IMG_TYPE type) {
		if (originalImg.empty()) {
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

	void release() {
		originalImg.release(); // 释放原始图像
	}
};		

