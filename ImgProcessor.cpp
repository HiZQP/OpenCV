#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic() {
	int total = 0;
	for (int i = 0; i < size_x; ++i) {
		uchar* row_ptr = grayImg.ptr<uchar>(i);
		for (int j = 0; j < size_y; ++j) {
			total += row_ptr[j]; // 累加每个像素的灰度值
		}
	}
	int avg = total / (size_x * size_y); // 计算平均灰度值
	// 设置HSV范围
	//函数拟合
	int a = 2; // 调整系数
	int lower = avg * a;
	return lower;
	return 0;
}

