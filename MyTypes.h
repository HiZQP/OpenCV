#include <opencv2/opencv.hpp>
#pragma once
#ifndef MY_TYPES_H
#define MY_TYPES_H
//����һ���Զ�������Ͷ����ļ����������������һЩ���߰������������

struct HSVRanges {
	cv::Scalar lower; // HSV����
	cv::Scalar upper; // HSV����
};

#endif // !MY_TYPES_H
