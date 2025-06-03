#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic(cv::Mat img) {
	int total = 0;
	int non_zero_count = 0;
	for (int i = 0; i < img.rows; i++) {
		uchar* row_ptr = img.ptr<uchar>(i);
		for (int j = 0; j < img.rows; j++) {
			if (row_ptr[j]) {
				total += row_ptr[j]; // 累加每个像素的灰度值
				non_zero_count++; // 统计非零像素的数量
			}
		}
	}
	int avg = total / (non_zero_count ? non_zero_count : 1); // 计算平均值，避免除以零
	// 设置HSV范围
	//函数拟合

	int lower = -4.2 * avg + 456;

	return lower;
	return 0;
}

