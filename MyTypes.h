#include <opencv2/opencv.hpp>
#pragma once
#ifndef MY_TYPES_H
#define MY_TYPES_H
//����һ���Զ�������Ͷ����ļ����������������һЩ���߰������������

struct HSVRanges {
	cv::Scalar lower; // HSV�½�
	cv::Scalar upper; // HSV�Ͻ�
	HSVRanges(cv::Scalar l, cv::Scalar u) : lower(l), upper(u) {} // ���캯��
};

#endif // !MY_TYPES_H
