#include <opencv2/opencv.hpp>
#include <vector>
#include <sstream>
#include <iostream>

using namespace cv;
using namespace std;

void display_info(Mat img) {
    //显示文本
    string  text = "Hello OpenCV!";// 文本内容
    Point textOrg(20, 20);// 文本位置，左上角坐标
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;// 字体类型
    double fontscale = 0.5;// 字体大小
    Scalar color(10, 128, 255);// 颜色，BGR格式
    int thickness = 1;// 线条粗细
    int baseline = 0;// 基线高度
    //背景图像，文本，文本位置，字体，字体大小，颜色，线条粗细
    //putText(img, text, textOrg, fontFace, fontscale, color, thickness);// 在图像上绘制文本

    //获取图片信息
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();
    Size imgSize = img.size();
    int depth = img.depth();

    vector<string> infoLines;// 用于存储信息的字符串向量
    infoLines.push_back("//TEST//");
    infoLines.push_back(to_string(height) + "x" + to_string(width));
    infoLines.push_back("Channels:" + to_string(channels));
    infoLines.push_back("depth:" + to_string(depth));
    //显示图片信息
    //计算所有文字间距
    Size sz = getTextSize(infoLines[0], fontFace, fontscale, thickness, &baseline);
    int lineSpacing = 5; // 行间距
    int Height = sz.height + lineSpacing; // 总高度
    //显示文本
    for (size_t i = 0;i < infoLines.size(); ++i) {
        putText(img, infoLines[i], Point(20, 20 + i * Height), fontFace, fontscale, color, thickness);
    }
}
void find_red(Mat resizedImg,Mat& redImg){
    Mat HSVImg;
    cvtColor(resizedImg, HSVImg, COLOR_BGR2HSV);//转换为HSV色彩空间
    // 定义颜色范围（这里以提取红色为例）
    Scalar lower_red1(0, 60, 150);    // HSV下限1
    Scalar upper_red1(10, 255, 255);  // HSV上限1
    Scalar lower_red2(170, 60, 150);   // HSV下限2（红色在HSV中跨越0°和180°）
    Scalar upper_red2(180, 255, 255); // HSV上限2
    Mat mask1, mask2, redMask;
    inRange(HSVImg, lower_red1, upper_red1, mask1);
    inRange(HSVImg, lower_red2, upper_red2, mask2);
    bitwise_or(mask1, mask2, redMask); // 合并两个范围的掩码
    // 应用掩码提取红色区域
    bitwise_and(resizedImg, resizedImg, redImg, redMask);
}

int main() {
    // 读取图片
    Mat img = imread("Resources/rmARMR.jpg");  // 替换为你的图片路径
    if (img.empty()) {
        cout << "图片加载失败！" << endl;
        return -1;
    }
    //图片处理
    Mat bulurredImg;
    GaussianBlur(img, bulurredImg, Size(5, 5), 1);//高斯模糊
    Mat resizedImg;
	resize(bulurredImg, resizedImg, Size(500, 500));//调整大小
	Mat redImg;
	find_red(resizedImg, redImg); // 提取红色区域
    Mat grayImg;
    cvtColor(redImg, grayImg, COLOR_BGR2GRAY);//转换为灰度图
    Mat binaryOriginalImg;
    int a = 100;
    int b = 300;
	Canny(resizedImg, binaryOriginalImg, a, b);//边缘检测
	//二值化处理
    Mat crt;
	threshold(grayImg, crt, 0, 255, THRESH_BINARY | THRESH_OTSU); // Otsu's 二值化

    //直线检测
    vector<Vec4i> lines;
    HoughLinesP(crt, lines, 1, CV_PI / 180, 20, 10, 4);
	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(resizedImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 2, LINE_AA); // 绘制检测到的直线
	}

    //显示图片
    display_info(grayImg);
	imshow("originalImage", resizedImg);
	imshow("binaryImage", crt);
	imshow("binaryOriginalImage", binaryOriginalImg);

    waitKey(0);  // 按任意键关闭窗口

    return 0;
}