#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic(ImgProcessor IP) {
	int total = 0;
	for (int i = 0; i < IP.size_x; ++i) {
		uchar* row_ptr = IP.grayImg.ptr<uchar>(i);
		for (int j = 0; j < IP.size_y; ++j) {
			total += row_ptr[j]; // 累加每个像素的灰度值
		}
	}
	int avg = total / (IP.size_x * IP.size_y); // 计算平均灰度值
	// 设置HSV范围
	//函数拟合
	int a = 2; // 调整系数
	int lower = avg * a;
	return lower;
	return 0;
}

