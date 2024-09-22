#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()

{
    Mat canvas = Mat::ones(300, 500, CV_8UC3) * 255;
    Scalar backGroundColor(48, 48, 48);

    vector<Vec4i> lines = {
        {190, 97, 303, 97},
        {228, 135, 303, 135},
        {311, 141, 353, 190},
        {160, 200, 190, 97},
        {245, 177, 349, 230},
        {245, 69, 303, 69},
        {257, 141, 287, 177},
        {210, 200, 228, 135},
        {257, 141, 311, 141},
        {152, 200, 210, 200},
        {212, 92, 265, 92},
        {315, 190, 349, 230},
        {245, 69, 265, 92},
        {166, 35, 186, 65},
        {315, 190, 353, 190},
        {246, 177, 287, 177},
        {186, 65, 212, 92},
        {166, 35, 303, 35}};

    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i l = lines[i];
        line(canvas, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 0), 1, LINE_AA);
    }

    ellipse(canvas, Point(303, 83), Size(12, 12), 0, -90, 90, backGroundColor, 1);
    ellipse(canvas, Point(303, 85), Size(52, 52), 0, -90, 90, backGroundColor, 1);

    // 漫水填充背景色
    floodFill(canvas, Point(240, 230), backGroundColor);
    floodFill(canvas, Point(290, 10), backGroundColor);

    Mat hsvCanvas;
    cvtColor(canvas, hsvCanvas, COLOR_BGR2HSV);

    Scalar lower_blue(100, 150, 0);
    Scalar upper_blue(140, 255, 255);

    // 提取蓝色区域和其边界(即主体图标区域、边界)
    Mat blueMask;
    inRange(hsvCanvas, lower_blue, upper_blue, blueMask);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(blueMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Vec3b start_color(0, 0, 255);
    Vec3b end_color(255, 0, 0);

    for (size_t i = 0; i < contours.size(); i++)
    {
        Rect boundingBox = boundingRect(contours[i]);

        // 对边界框内的每个像素应用渐变色填充
        for (int row = boundingBox.y; row < boundingBox.y + boundingBox.height; ++row)
        {
            for (int col = boundingBox.x; col < boundingBox.x + boundingBox.width; ++col)
            {
                // 判断该像素是否在轮廓内
                if (pointPolygonTest(contours[i], Point(col, row), false) >= 0)
                {
                    // 计算渐变比例和对应颜色并逐像素设置颜色（根据横坐标进行水平渐变）
                    float alpha = static_cast<float>(col - boundingBox.x) / static_cast<float>(boundingBox.width);
                    Vec3b color = (0.95 - alpha) * start_color + (0.05 + alpha) * end_color;
                    canvas.at<Vec3b>(row, col) = color;
                }
            }
        }
    }

    string text = "RoboMaster";
    int fontFace = FONT_HERSHEY_DUPLEX | FONT_ITALIC;
    double fontScale = 1.4;
    int thickness = 2;
    int baseline = 0;
    int width = 500;
    int height = 300;
    int ksize = 31;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
    Point textOrg((width - textSize.width) / 2, height - textSize.height);

    Mat textGlow = Mat::zeros(canvas.size(), canvas.type());
    putText(textGlow, text, Point(textOrg.x + 3, textOrg.y + 3), fontFace, fontScale, Scalar(255, 156, 32), thickness + 1, LINE_AA);

    Mat textGlowBlurred;
    GaussianBlur(textGlow, textGlowBlurred, Size(ksize, ksize), 0, 0);

    addWeighted(canvas, 1.0, textGlowBlurred, 1.0, 0, canvas);
    putText(canvas, text, textOrg, fontFace, fontScale, Scalar(255, 255, 255), thickness, LINE_AA);
    imshow("logo", canvas);
    imwrite("../resources/robomaster_logo.png", canvas);

    waitKey(0);
    return 0;
}