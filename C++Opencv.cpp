#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>

using namespace cv;
using namespace std;
//颜色检测非常依赖相机质量，不同相机对于相同颜色的HSV范围差异天壤之别
//rmTest和rmTest1的拍摄相机不同，两者的HSV范围完全不能通用
void find_red(Mat resizedImg, Mat& redImg) {
    Mat HSVImg;
    cvtColor(resizedImg, HSVImg, COLOR_BGR2HSV);//转换为HSV色彩空间
    // 定义颜色范围H:色相S:饱和度V:亮度（这里以提取红色为例）
    Scalar lower_red1(0, 0, 180);    // HSV下限1
    Scalar upper_red1(40, 120, 255);  // HSV上限1
    Scalar lower_red2(150, 0, 180);   // HSV下限2（红色在HSV中跨越0°和180°）
    Scalar upper_red2(180, 120, 255); // HSV上限2
    Mat mask1, mask2, redMask;
    inRange(HSVImg, lower_red1, upper_red1, mask1);
    inRange(HSVImg, lower_red2, upper_red2, mask2);
    bitwise_or(mask1, mask2, redMask); // 合并两个范围的掩码
    // 应用掩码提取红色区域
    bitwise_and(resizedImg, resizedImg, redImg, redMask);
}
void preprocess_image(string imgPath, Mat& grayImg, Mat& resizedImg, Mat& crt) {
    // 读取图片
    Mat img = imread(imgPath);  // 替换为你的图片路径
    if (img.empty()) {
        cout << "图片加载失败！" << endl;
        return ;
    }
    //图片处理
    Mat bulurredImg;
    GaussianBlur(img, bulurredImg, Size(13, 21), 1);//高斯模糊
    resize(bulurredImg, resizedImg, Size(500, 500));//调整大小
    Mat redImg;
    find_red(resizedImg, redImg); // 提取红色区域
    cvtColor(redImg, grayImg, COLOR_BGR2GRAY);//转换为灰度图
	//去除孤立小点
	Mat betterImg;
    morphologyEx(grayImg, betterImg, cv::MORPH_OPEN, 2);
	//二值化处理
    int a = 100;
    int b = 300;
    //Canny(resizedImg, binaryOriginalImg, a, b);//边缘检测
    threshold(betterImg, crt, 0, 255, THRESH_BINARY | THRESH_OTSU); // Otsu's 二值化
}
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
void find_target(Mat& resizedImg,Mat& crt,vector<Point>& tergets) {
    //直线检测与绘制
    vector<Vec4i> detected_lines;
    HoughLinesP(crt, detected_lines, 1, CV_PI / 180, 8, 8, 1);
    /*
    for (size_t i = 0; i < detected_lines.size(); i++) {
        Vec4i l = detected_lines[i];
        line(resizedImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 2, LINE_AA); // 绘制检测到的直线
    }
    */
	//合并接近的直线
	vector<Vec4i> merged_lines;
	vector<int> merged_indices; // 用于存储已合并的直线索引
    for (int i = 0;i < detected_lines.size();i++) {
		// 检查当前直线是否已被合并
		if (find(merged_indices.begin(), merged_indices.end(), i) != merged_indices.end()) {
			continue; // 如果已合并，则跳过
		}
		// 否则，开始合并过程
		int isMerged = 0; // 标记是否已合并
        for (int j = i + 1;j < detected_lines.size();j++) {
            Vec4i l = detected_lines[i];
            Vec4i m = detected_lines[j];
            // 如果直线两端点距离很近，则合并这两条直线
            double distance1 = sqrt(pow(l[0] - m[0], 2) + pow(l[1] - m[1], 2));
            double distance2 = sqrt(pow(l[2] - m[2], 2) + pow(l[3] - m[3], 2));
            if (distance1 < 2 && distance2 < 2) {
				// 计算合并后的直线端点
				int x1 = min(l[0], m[0]);
				int y1 = min(l[1], m[1]);
				int x2 = max(l[2], m[2]);
				int y2 = max(l[3], m[3]);
				Vec4i merged_line(x1, y1, x2, y2);
				merged_lines.push_back(merged_line);
				isMerged = 1; // 标记已合并
				merged_indices.push_back(j); // 添加已合并的直线索引
			}
        }
		// 如果当前直线没有被合并，则添加到合并后的直线列表中
		if (!isMerged) {
			merged_lines.push_back(detected_lines[i]);
		}
    }
    
    for (size_t i = 0; i < merged_lines.size(); i++) {
        Vec4i l = merged_lines[i];
        line(resizedImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 2, LINE_AA); // 绘制检测到的直线
    }
    
    //选择接近垂直的直线
    vector<Vec4i> vertical_lines;
    for (size_t i = 0; i < merged_lines.size(); i++) {
        Vec4i l = merged_lines[i];
        double angle = atan2(l[3] - l[1], l[2] - l[0]) * 180 / CV_PI; // 计算直线的角度
        if (abs(angle) < 110 && abs(angle) > 70) { // 接近垂直的直线
            vertical_lines.push_back(l);
        }
    }
    for (size_t i = 0; i < vertical_lines.size(); i++) {
        Vec4i l = vertical_lines[i];
        line(resizedImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 255), 2, LINE_AA); // 绘制检测到的直线
    }
	int targetCount = 0; // 目标计数
    int lineCount = vertical_lines.size();
    for (int i = 0;i < lineCount;i++) {
        for (int j = i + 1;j < lineCount;j++) {
            Vec4i l = vertical_lines[i];
            Vec4i m = vertical_lines[j];
            double midX1 = (l[0] + l[2]) / 2.0; // 直线l的中点X坐标
            double midY1 = (l[1] + l[3]) / 2.0; // 直线l的中点Y坐标
            double midX2 = (m[0] + m[2]) / 2.0; // 直线m的中点X坐标
            double midY2 = (m[1] + m[3]) / 2.0; // 直线m的中点Y坐标
            double distance = sqrt(pow(midX2 - midX1, 2) + pow(midY2 - midY1, 2)); // 计算中点间距离
            double rangemin = sqrt(pow(l[0] - l[2], 2) + pow(l[1] - l[3], 2)) * 1.4;
            double rangemax = rangemin * 2;
            if (distance < rangemax && distance >rangemin && abs(l[1] - m[1]) < 3 && abs(l[3] - m[3]) < 3) { // 如果距离小于阈值，则连接这两条直线的中点
                Point midPoint1(midX1, midY1);
                Point midPoint2(midX2, midY2);
                line(resizedImg, midPoint1, midPoint2, Scalar(0, 255, 0), 2, LINE_AA); // 绘制连接线
                Point aimPoint((midX1 + midX2) / 2, (midY1 + midY2) / 2);
                int size = 10; // 线段长度
				line(resizedImg, Point(aimPoint.x - size, aimPoint.y + size), Point(aimPoint.x + size, aimPoint.y - size), Scalar(0, 0, 255), 2, LINE_AA);// 绘制十字线
                line(resizedImg, Point(aimPoint.x - size, aimPoint.y - size), Point(aimPoint.x + size, aimPoint.y + size), Scalar(0, 0, 255), 2, LINE_AA);
				putText(resizedImg, "terget" + to_string(++targetCount), Point(aimPoint.x - 25, aimPoint.y + 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1); // 在目标点旁边标记编号
				tergets.push_back(aimPoint); // 将目标点添加到结果向量中
            }
        }
    }
}
void front_sight(Mat& resizedImg,vector<Point> targets) {
	int centerX = resizedImg.cols / 2; // 图像中心X坐标
	int centerY = resizedImg.rows / 2; // 图像中心Y坐标
	int size = 10; // 十字线长度
	line(resizedImg, Point(centerX - size, centerY + size), Point(centerX + size, centerY - size), Scalar(255, 255, 255), 2, LINE_AA); // 绘制十字线
	line(resizedImg, Point(centerX - size, centerY - size), Point(centerX + size, centerY + size), Scalar(255, 255, 255), 2, LINE_AA);
	for (const auto& target : targets) {
		line(resizedImg, target, Point(centerX, centerY), Scalar(155, 155, 155), 2, LINE_AA); // 绘制目标点十字线
	}
}

int main() {
    // 预处理图像
	Mat grayImg, resizedImg, crt;
	preprocess_image("Resources/rmTest1.jpg", grayImg, resizedImg, crt); // 替换为你的图片路径
    vector<Point> targets;
	find_target(resizedImg, crt, targets); // 查找目标并绘制
	front_sight(resizedImg, targets); // 绘制准星
    //显示图片
    display_info(grayImg);
	imshow("originalImage", resizedImg);
	imshow("binaryImage", crt);
	imshow("grayImage", grayImg);

    waitKey(0);  // 按任意键关闭窗口

    return 0;
}