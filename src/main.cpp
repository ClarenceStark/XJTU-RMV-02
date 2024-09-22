#include <opencv2/opencv.hpp>
#include <iostream>
#include "box_painting.h"

using namespace cv;
using namespace std;

int main()
{
    // 读取图像
    Mat image = imread("../resources/test_image.png");
    if (image.empty())
    {
        cerr << "无法加载图像！" << endl;
        return -1;
    }

    // 转换为灰度图
    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    // 转换为HSV图片
    Mat hsv_image;
    cvtColor(image, hsv_image, COLOR_BGR2HSV);

    // 均值滤波
    Mat mean_blur;
    blur(image, mean_blur, Size(5, 5));

    // 高斯滤波
    Mat gaussian_blur;
    GaussianBlur(image, gaussian_blur, Size(5, 5), 0);

    // 提取红色颜色区域（HSV）(双掩膜合并)
    Mat red_mask;
    Scalar lower_red1(0, 100, 100);
    Scalar upper_red1(10, 255, 255);
    Scalar lower_red2(160, 100, 100);
    Scalar upper_red2(179, 255, 255);
    Mat mask1, mask2;
    inRange(hsv_image, lower_red1, upper_red1, mask1);
    inRange(hsv_image, lower_red2, upper_red2, mask2);
    bitwise_or(mask1, mask2, red_mask);

    // 寻找红色的外轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(red_mask, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    // 调用函数绘制红色bounding box（函数定义见box_painting.cpp）
    Mat bounding_box_image = paint_box(image.clone(), 5000, contours, 0.9);

    // 绘制红色的外轮廓
    Mat contour_image = image.clone();
    int count = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {
        // 计算轮廓面积并根据面积筛选掉过小的噪声轮廓，绘制轮廓 打印面积
        double area = contourArea(contours[i]);
        if (area > 100)
        {
            cout << "Contour " << count++ << " area: " << area << endl;
            drawContours(contour_image, contours, i, Scalar(0, 255, 0), 2);
        }
    }

    // 二值化（已有灰度图gray_image）
    Mat binary_image;
    threshold(gray_image, binary_image, 200, 255, THRESH_BINARY);

    Mat filled_image;
    cvtColor(binary_image, filled_image, COLOR_GRAY2BGR);

    // 漫水填充黑色区域
    Mat mask = Mat::zeros(binary_image.rows + 2, binary_image.cols + 2, CV_8UC1);
    floodFill(filled_image, mask, Point(500, 500), Scalar(0, 155, 0), 0, Scalar(), Scalar(), 4);

    // 膨胀
    Mat dilated_image;
    dilate(binary_image, dilated_image, Mat(), Point(-1, -1), 2);

    // 腐蚀
    Mat eroded_image;
    erode(binary_image, eroded_image, Mat(), Point(-1, -1), 2);

    // 图像旋转35度（有损）
    Point2f center(image.cols / 2.0F, image.rows / 2.0F);
    Mat rotation_matrix = cv::getRotationMatrix2D(center, 35, 1.0);
    Mat rotated_image;
    warpAffine(image, rotated_image, rotation_matrix, image.size());

    // 图像旋转35度（无损）
    double radians = 35 * CV_PI / 180.0;
    // 计算旋转后图像的宽高
    int new_width = static_cast<int>(image.cols * abs(cos(radians)) + image.rows * abs(sin(radians)));
    int new_height = static_cast<int>(image.cols * abs(sin(radians)) + image.rows * abs(cos(radians)));
    Mat rotation_matrix_lossless = getRotationMatrix2D(center, 35, 1.0);
    // 计算旋转矩阵偏移量
    rotation_matrix_lossless.at<double>(0, 2) += (new_width - image.cols) / 2.0;
    rotation_matrix_lossless.at<double>(1, 2) += (new_height - image.rows) / 2.0;
    Mat rotated_image_lossless;
    warpAffine(image, rotated_image_lossless, rotation_matrix_lossless, Size(new_width, new_height));

    // 图像裁剪为左上角1/4
    int crop_width = image.cols / 2;
    int crop_height = image.rows / 2;
    Rect roi(0, 0, crop_width, crop_height);
    Mat cropped_image = image(roi);

    imshow("Original Image", image);
    imshow("Gray Image", gray_image);
    imshow("HSV Image", hsv_image);
    imshow("Mean Blur", mean_blur);
    imshow("Gaussian Blur", gaussian_blur);
    imshow("Red Regions", red_mask);
    imshow("Highlight Regions", binary_image);
    imshow("Contours", contour_image);
    imshow("Bounding Boxes", bounding_box_image);
    imshow("Dilated Image", dilated_image);
    imshow("Eroded Image", eroded_image);
    imshow("Floodfilled Image", filled_image);
    imshow("Rotated Image", rotated_image);
    imshow("Rotated Image Lossless", rotated_image_lossless);
    imshow("Cropped Image", cropped_image);

    imwrite("../resources/gray_image.png", gray_image);
    imwrite("../resources/hsv_image.png", hsv_image);
    imwrite("../resources/mean_blur.png", mean_blur);
    imwrite("../resources/gaussian_blur.png", gaussian_blur);
    imwrite("../resources/red_mask.png", red_mask);
    imwrite("../resources/highlight_regions.png", binary_image);
    imwrite("../resources/contour_image.png", contour_image);
    imwrite("../resources/bounding_box_image.png", bounding_box_image);
    imwrite("../resources/dilated_image.png", dilated_image);
    imwrite("../resources/eroded_image.png", eroded_image);
    imwrite("../resources/filled_image.png", filled_image);
    imwrite("../resources/rotated_image.png", rotated_image);
    imwrite("../resources/rotated_image_lossless.png", rotated_image_lossless);
    imwrite("../resources/cropped_image.png", cropped_image);

    waitKey(0);

    return 0;
}
