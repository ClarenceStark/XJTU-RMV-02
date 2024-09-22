#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "box_painting.h"

using namespace cv;
using namespace std;

// 判断重叠函数（重叠度达overlappingRate以上返回）
Rect almostOverlapped(const Rect &rect1, const Rect &rect2, float overlappingRate)
{
    int intersectionArea = (rect1 & rect2).area();
    if ((float)intersectionArea / rect1.area() > overlappingRate)
    {
        return rect1;
    }
    if ((float)intersectionArea / rect2.area() > overlappingRate)
    {
        return rect2;
    }
    return Rect();
}

// 筛选bounding box的函数（防止有太多高度重合的冗余的bounding box）
vector<Rect> screening(const vector<Rect> &boxes, float overlappingRate)
{
    vector<Rect> result;
    vector<bool> disabled(boxes.size(), false);

    for (size_t i = 0; i < boxes.size(); ++i)
    {
        for (size_t j = i + 1; j < boxes.size(); ++j)
        {
            // 如果有框和当前框几乎重叠，则禁用那个框
            if (almostOverlapped(boxes[i], boxes[j], 0.9) == boxes[j])
            {
                disabled[j] = true;
            }
            else if (almostOverlapped(boxes[i], boxes[j], 0.9) == boxes[i]) // 如果当前框几乎与另一个框重合，则禁用当前框
            {
                disabled[i] = true;
                break; // 当前框被筛除后，就跳出该框的循环
            }
        }
        if (disabled[i])
            continue;
        result.push_back(boxes[i]);
    }
    return result;
}

Mat paint_box(Mat img, double min_area_threshold, vector<vector<Point>> contours, float overlappingRate)
{
    vector<Rect> boxes;
    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > min_area_threshold)
        { // 过滤掉面积较小的轮廓
            Rect rect = boundingRect(contours[i]);
            boxes.push_back(rect);
        }
    }

    // 用筛选函数去除高度重叠的冗余矩形框
    vector<Rect> final_boxes = screening(boxes, overlappingRate);

    Mat bounding_box_image = img.clone();

    for (const Rect &rect : final_boxes)
    {
        // 绘制矩形框
        rectangle(bounding_box_image, rect, Scalar(255, 0, 0), 2);
    }
    return bounding_box_image;
}