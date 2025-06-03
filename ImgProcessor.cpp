#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic(ImgProcessor IP) {
	int total = 0;
	for (int i = 0; i < IP.size_x; ++i) {
		uchar* row_ptr = IP.grayImg.ptr<uchar>(i);
		for (int j = 0; j < IP.size_y; ++j) {
			total += row_ptr[j]; // �ۼ�ÿ�����صĻҶ�ֵ
		}
	}
	int avg = total / (IP.size_x * IP.size_y); // ����ƽ���Ҷ�ֵ
	// ����HSV��Χ
	//�������
	int a = 2; // ����ϵ��
	int lower = avg * a;
	return lower;
	return 0;
}

