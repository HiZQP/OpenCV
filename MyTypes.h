#include <opencv2/opencv.hpp>
#pragma once
#ifndef MY_TYPES_H
#define MY_TYPES_H
//这是一个自定义的类型定义文件，你可以在这里整一些乱七八糟的数据类型

struct HSVRanges {
	cv::Scalar lower; // HSV下限
	cv::Scalar upper; // HSV上限
};

#endif // !MY_TYPES_H
