/*
* ����һ��ͼ��Ԥ�����ֱ࣬��ʹ��OpenCV��ĺ���һ����һ���ǵ��е㷳��
* Ϊ�˷����д�������⡣
* 
* д����ע����Ҫ�����Լ�������ô�á�
* 
* ˳������һ���Լ�����Ҫ��ͷ�ļ�ʹ��using namespace�������Ⱦȫ�������ռ䡣
* 
* �����ֱ��ʹ��preprocess_image����������ͼ��
* Ŀǰ������
* 1. ����ͼ��
* 2. �Ҷ�ͼ��
* 3. ��ֵͼ��
* 4. HSV����ͼ��
* 
* ʹ��ʱֻ���ṩͼƬ·��������Ĵ������ͼ��ɡ�
* ע�⣺
* 1.����ͼ��ͻҶ�ͼ���ǻ���ԭʼͼ��ģ���ֵͼ���ǻ��ڻҶ�ͼ��ġ�
* 2.HSV������Ҫ�ṩHSV��Χ����������������е�HSVRangers�ṹ�嶨����MyTypes.h�С�
*/
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <vector>
#include "MyTypes.h"

#pragma once
class ImgProcessor {
private:

	std::string imgPath; // ͼƬ·��
	cv::Mat originalImg; // ԭʼͼ��
	int size_x, size_y; // ͼ��ߴ�

public:

	static enum IMG_TYPE {
		RESIZED_IMG = 0, // ���ź��ͼ��
		GRAY_IMG,       // �Ҷ�ͼ��
		BINARY_IMG,        // ��ֵͼ��
		HSV_FILTERED_IMG // HSV���˺��ͼ��
	};

	ImgProcessor(const std::string path, const int size_x, const int size_y) : imgPath(path), size_x(size_x), size_y(size_y){
		originalImg = cv::imread(imgPath); // ��ȡͼƬ
	}
	~ImgProcessor() {}

	cv::Mat preprocess_image(IMG_TYPE type, std::vector<HSVRanges> hsvRanges) {
		if (originalImg.empty()) {
			std::cout << "ͼƬ����ʧ�ܣ�" << std::endl;
			return cv::Mat(); // ���ؿվ���
		}
		switch (type) {
		case HSV_FILTERED_IMG: {
			cv::Mat hsvFilteredImg = originalImg.clone();
			for (const auto& range : hsvRanges) {
				cv::inRange(originalImg, range.lower, range.upper, hsvFilteredImg); // Ӧ��HSV��Χ����
			}
			return hsvFilteredImg;
		}
		default:
			std::cout << "δ֪���ͣ�" << std::endl; 
			return cv::Mat(); // ���ؿվ���
		}
	}

	cv::Mat preprocess_image(IMG_TYPE type) {
		cv::Mat img = cv::imread(imgPath); // ��ȡͼƬ
		if (img.empty()) {
			std::cout << "ͼƬ����ʧ�ܣ�" << std::endl;
			return cv::Mat();
		}
		switch (type) {
		case RESIZED_IMG: {
			cv::Mat resizedImg;
			cv::resize(originalImg, resizedImg, cv::Size(size_x, size_y)); // ����ͼ��
			return resizedImg;
		}
		case GRAY_IMG: {
			cv::Mat grayImg;
			cv::Mat resizedImg = preprocess_image(RESIZED_IMG); // ������ͼ��
			cv::cvtColor(resizedImg, grayImg, cv::COLOR_BGR2GRAY); // ת��Ϊ�Ҷ�ͼ��
			return grayImg;
		}
		case BINARY_IMG: {
			cv::Mat grayImg = preprocess_image(GRAY_IMG);
			cv::Mat binaryImg;
			cv::threshold(grayImg, binaryImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // Otsu's ��ֵ��
			return binaryImg;
		}
		default:
			std::cout << "δ֪���ͣ�" << std::endl;
			return cv::Mat(); // ���ؿվ���
		}
	}

	cv::Mat preprocess_image() {
		return preprocess_image(RESIZED_IMG); // Ĭ�Ϸ������ź��ͼ��
	}
};

