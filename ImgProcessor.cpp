#include "ImgProcessor.h"

int ImgProcessor::HSVDynamic() {
	int total = 0;
	for (int i = 0; i < size_x; ++i) {
		uchar* row_ptr = grayImg.ptr<uchar>(i);
		for (int j = 0; j < size_y; ++j) {
			total += row_ptr[j]; // �ۼ�ÿ�����صĻҶ�ֵ
		}
	}
	int avg = total / (size_x * size_y); // ����ƽ���Ҷ�ֵ
	// ����HSV��Χ
	//�������
	int a = 2; // ����ϵ��
	int lower = avg * a;
	return lower;
	return 0;
}

