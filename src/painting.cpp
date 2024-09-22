#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    int width = 800;
    int height = 600;
    Mat image(height, width, CV_8UC3, Scalar(45, 45, 45));

    // 矩形参数
    Point square_center(width / 4, height / 2);
    int square_size = 100;
    Point top_left(square_center.x - square_size / 2, square_center.y - square_size / 2);
    Point bottom_right(square_center.x + square_size / 2, square_center.y + square_size / 2);

    // 圆的参数
    Point circle_center(3 * width / 4, height / 2);
    int circle_radius = 50;

    Mat glow_image = Mat::zeros(image.size(), image.type());

    rectangle(glow_image, Point(top_left.x + 4, top_left.y + 4), Point(bottom_right.x + 4, bottom_right.y + 4), Scalar(100, 255, 0), -1, LINE_AA);
    circle(glow_image, Point(circle_center.x + 4, circle_center.y + 4), circle_radius, Scalar(28, 60, 251), -1, LINE_AA);

    // 高斯模糊模拟阴影
    Mat glow;
    int ksize = 71;
    GaussianBlur(glow_image, glow, Size(ksize, ksize), 0, 0);

    Mat result;
    addWeighted(image, 1.0, glow, 1.0, 0, result);

    // 叠加实的矩形，圆形
    rectangle(result, top_left, bottom_right, Scalar(100, 255, 0), -1, LINE_AA);
    circle(result, circle_center, circle_radius, Scalar(28, 60, 251), -1, LINE_AA);

    // 文字部分处理同矩形、圆形
    string text = "RoboMaster";
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 2.0;
    int thickness = 3;
    int baseline = 0;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
    Point textOrg((width - textSize.width) / 2, height - textSize.height);

    Mat textGlow = Mat::zeros(image.size(), image.type());
    putText(textGlow, text, Point(textOrg.x + 3, textOrg.y + 3), fontFace, fontScale, Scalar(255, 156, 32), thickness + 1, LINE_AA);

    Mat textGlowBlurred;
    GaussianBlur(textGlow, textGlowBlurred, Size(41, 41), 0, 0);

    addWeighted(result, 1.0, textGlowBlurred, 1.0, 0, result);
    putText(result, text, textOrg, fontFace, fontScale, Scalar(255, 255, 255), thickness, LINE_AA);

    imwrite("../resources/rectangle_circle_text.png", result);

    // 图像转换为HSV
    Mat hsvImage;
    cvtColor(result, hsvImage, COLOR_BGR2HSV);

    // 红色HSV阈值范围
    Mat mask1, mask2;
    inRange(hsvImage, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1);
    inRange(hsvImage, Scalar(160, 100, 100), Scalar(180, 255, 255), mask2);

    Mat redMask;
    bitwise_or(mask1, mask2, redMask);

    // 查找红色区域的轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(redMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 绘制轮廓和bounding box
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(result, contours, (int)i, Scalar(0, 255, 0), 2);

        Rect boundingBox = boundingRect(contours[i]);
        rectangle(result, boundingBox, Scalar(255, 0, 0), 2);
    }

    imwrite("../resources/rectangle_circle_text_red.png", result);

    imshow("result", result);
    waitKey(0);

    return 0;
}
