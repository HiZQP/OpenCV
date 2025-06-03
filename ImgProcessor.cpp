#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic(cv::Mat img) {
	int total = 0;
	int non_zero_count = 0;
	for (int i = 0; i < img.rows; i++) {
		uchar* row_ptr = img.ptr<uchar>(i);
		for (int j = 0; j < img.rows; j++) {
			if (row_ptr[j]) {
				total += row_ptr[j]; // �ۼ�ÿ�����صĻҶ�ֵ
				non_zero_count++; // ͳ�Ʒ������ص�����
			}
		}
	}
	int avg = total / (non_zero_count ? non_zero_count : 1); // ����ƽ��ֵ�����������
	// ����HSV��Χ
	//�������

	int lower = -4.2 * avg + 456;

	return lower;
	return 0;
}

