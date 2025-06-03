#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
#include "ImgProcessor.h"
#include "MyTypes.h"

using namespace cv;
using namespace std; 
//颜色检测严重依赖相机质量，不同相机对于相同颜色的HSV范围差异天壤之别
//rmTest和rmTest1的拍摄相机不同，两者的HSV范围几乎完全不能通用
/* 这是旧的颜色检测函数，请使用类ImgProcessor中的HSV过滤函数
void find_red(Mat resizedImg, Mat& redImg) {
    Mat HSVImg;
    cvtColor(resizedImg, HSVImg, COLOR_BGR2HSV);//转换为HSV色彩空间
    // 定义颜色范围 H:色相 S:饱和度 V:亮度（这里以提取红色为例）
    Scalar lower_red1(0, 0, 180);    // HSV下限1
    Scalar upper_red1(40, 120, 255);  // HSV上限1
    Scalar lower_red2(160, 0, 180);   // HSV下限2（红色在HSV中跨越0°和180°）
    Scalar upper_red2(180, 120, 255); // HSV上限2
    Mat mask1, mask2, redMask;
    inRange(HSVImg, lower_red1, upper_red1, mask1);
    inRange(HSVImg, lower_red2, upper_red2, mask2);
    bitwise_or(mask1, mask2, redMask); // 合并两个范围的掩码
    // 应用掩码提取红色区域
    bitwise_and(resizedImg, resizedImg, redImg, redMask);
}
void find_numberColor(Mat resizedImg, Mat& redImg) {
    Mat HSVImg;
    cvtColor(resizedImg, HSVImg, COLOR_BGR2HSV);//转换为HSV色彩空间
    // 定义颜色范围 H:色相 S:饱和度 V:亮度（这里以提取红色为例）
    Scalar lower_red1(80, 0, 0);    // HSV下限1
    Scalar upper_red1(180, 45, 255);  // HSV上限1
    Mat mask1, mask2, redMask;
    inRange(HSVImg, lower_red1, upper_red1, mask1);
    // 应用掩码提取红色区域
    bitwise_and(resizedImg, resizedImg, redImg, mask1);
}
*/
// 显示图片信息 (信息处理/绘制 输入/输出)
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
// 查找目标并绘制 (信息绘制输出， 信息处理输入（二值图）， 目标输出（点向量）)
void find_target(Mat& resizedImg, Mat& binaryImg, vector<Point>& targets) {
    vector<vector<Point>> contours;
	vector<vector<Point>> somethingLikeTargetContours;
    vector<Vec4i> hierarchy;
    findContours(binaryImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // 查找轮廓
    // 绘制轮廓
    for (size_t i = 0; i < contours.size(); i++) {
        Rect boundingBox = boundingRect(contours[i]); // 获取轮廓的边界矩形
        if ((float)boundingBox.height / (float)boundingBox.width > 1.85 && boundingBox.height * boundingBox.width > 25) {
            rectangle(resizedImg, boundingBox, Scalar(255, 0, 0), FILLED);
			somethingLikeTargetContours.push_back(contours[i]); // 将符合条件的轮廓添加到目标轮廓列表
        }
    }
    int targetCount = 0;
	for (size_t i = 0; i < somethingLikeTargetContours.size(); i++) {
		for (size_t j = i + 1; j < somethingLikeTargetContours.size(); j++) {
			// 计算两个轮廓的中心点
			Rect boundingBox1 = boundingRect(somethingLikeTargetContours[i]);
			Rect boundingBox2 = boundingRect(somethingLikeTargetContours[j]);
			Point center1(boundingBox1.x + boundingBox1.width / 2, boundingBox1.y + boundingBox1.height / 2);
			Point center2(boundingBox2.x + boundingBox2.width / 2, boundingBox2.y + boundingBox2.height / 2);
			// 计算中心点之间的距离
			double distance = norm(center1 - center2);
			// 计算两个轮廓平均高度
			double averageHeight = (boundingBox1.height + boundingBox2.height) / 2.0;
			double minDistance = averageHeight * 1.2;
            double maxDistance = minDistance * 1.2;
            if (distance > minDistance && distance < maxDistance && abs(center1.y - center2.y) < averageHeight / 5) {
                line(resizedImg, center1, center2, Scalar(255, 0, 0), 2, LINE_AA); // 绘制连接线
				int size = 10; // 十字线长度
				Point target((center1.x + center2.x) / 2, (center1.y + center2.y) / 2);
				line(resizedImg, Point(target.x - size, target.y + size), Point(target.x + size, target.y - size), Scalar(0, 0, 255), 2, LINE_AA); // 绘制十字线
				line(resizedImg, Point(target.x - size, target.y - size), Point(target.x + size, target.y + size), Scalar(0, 0, 255), 2, LINE_AA);
				putText(resizedImg, "TGT_" + to_string(++targetCount), Point(target.x - 20,target.y - 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
				targets.push_back(target); // 将目标点添加到目标列表
            }
        }
	}
}
// 绘制准星 (信息处理输入，目标输入（点向量）)
void front_sight(Mat& resizedImg,vector<Point> targets) {
	int centerX = resizedImg.cols / 2; // 图像中心X坐标
	int centerY = resizedImg.rows / 2; // 图像中心Y坐标
	for (const auto& target : targets) {
		line(resizedImg, target, Point(centerX, centerY), Scalar(155, 155, 155), 2, LINE_AA); // 绘制目标点十字线
	}
    int size = 18; // 十字线长度
    circle(resizedImg, Point(centerX, centerY), size, Scalar(0, 255, 0), 1, LINE_AA); // 绘制中心圆
}

int main() {

    // 预处理图像
    //(图片路径输入，缩放尺寸x输入，缩放尺寸y输入 )
	ImgProcessor preprocessor1("Resources/rmTest1.jpg", 500, 500);
    Mat grayImg = preprocessor1.grayImg;
    Mat resizedImg = preprocessor1.resizedImg;
    Mat binaryImg = preprocessor1.binaryImg;

	// 目前发现HSV亮度下限与图片整体亮度相关，用函数将两者拟合可以提高颜色过滤的准确性
	// preprocessor1.HSVDynamic(preprocessor1); // 动态计算HSV亮度下限
    HSVRanges redRange1(Scalar(0, 0, 200), Scalar(20, 255, 255)); // 定义红色HSV范围
    HSVRanges redRange2(Scalar(170, 0, 200), Scalar(180, 255, 255));

    HSVRanges blueRange(Scalar(80, 0, 0), Scalar(180, 45, 255)); // 定义蓝色HSV范围
	vector<HSVRanges> redRanges = { redRange1, redRange2 }; // 将红色范围放入向量中
	preprocessor1.preprocess_image(ImgProcessor::HSV_FILTERED_IMG, redRanges); // 使用HSV过滤函数

    vector<Point> targets;
    find_target(resizedImg, preprocessor1.filteredBinaryImg, targets); // 查找目标并绘制
	front_sight(resizedImg, targets); // 绘制准星
	//imwrite("Output/processedImage.jpg", resizedImg); // 保存处理后的图像
    
    //显示图片
    display_info(resizedImg);
	imshow("originalImage", resizedImg);
	imshow("binaryImage", binaryImg);
	imshow("redFilteredImage", preprocessor1.filteredBinaryImg); // 显示红色过滤后的图像
    waitKey(0);  // 按任意键关闭窗口

    return 0;
}