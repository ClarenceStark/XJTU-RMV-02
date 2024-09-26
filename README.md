## 总体思路

**在完成任务前，我首先粗略浏览了OpenCV的官方文档和教程，对库的功能和结构有了一个初步的了解。每当在实现过程中遇到不明确的概念或函数，我都会google相关的实现方式，参考他人的经验和代码示例，为我提供了实用的解决方案和思路。**

**在获取了基本的实现方法后，我会回到OpenCV的官方文档，仔细阅读相关函数的详细说明和参数解释。更深入地理解函数的工作原理和适用场景。随后将学到的知识应用到实际代码中，根据任务的具体要求进行编写和调整。**

**为了达到最佳的效果，不断地在代码中进行测试和优化。通过调整参数、修改算法和尝试不同的处理方法，逐步提高程序的性能和结果的准确性。在这个过程中，我不仅解决了实际的问题，也加深了对图像处理技术和OpenCV库的理解。为后续实现更高级的图像处理任务打下了基础。**

## 具体任务实现思路

### 1. 图像读取与颜色空间转换和滤波操作

- 通过查询相应函数并调用实现相应操作。（`cvtColor`、`blur`、`GuassianBlur`）

### 2. 红色区域提取与轮廓检测

- 由于红色在 HSV 空间中分布在两个区域，分别使用 `inRange` 创建两个掩膜，然后使用 `bitwise_or` 合并。使用`findContours(red_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE)`查找轮廓并用`drawContours`绘制，一开始`drawContours(contour_image, contours, -1, Scalar(0, 255, 0), 2)`绘制出所有轮廓，发现有许多微小噪点，打印轮廓面积发现有许多面积接近于0甚至为0的轮廓，并且内部轮廓没有提取出来（比如腿部的黑色网格处），查找发现是轮廓提取模式RETR_EXTERNAL只提取外部轮廓，将其换为RETR_LIST即可提取内部轮廓，并且，通过设定阈值并且循环检查的方式来过滤掉面积过小的轮廓并绘制，得到干净的红色轮廓。

### 3. 绘制红色的 Bounding Box

- 首先，通过 `boundingRect(contours[i])` 计算经过过滤后的轮廓的边界矩形，并将其存储在一个矩形框列表中。遍历列表使用`rectangle`绘制bounding box，发现绘制出的矩形框会覆盖在多个重叠的区域上，图像中出现许多冗余的框。这些框不仅重叠在一起，造成视觉上的混乱，还使得轮廓的实际信息难以辨识，尤其是在密集的红色区域中。

  为了避免这一点，我自定义函数 `screening` 进行重叠筛选。通过计算矩形框的交集面积与原始框的面积比值，判断重叠度是否超过设定的阈值，并对矩形面积设定阈值进行进一步筛选，确保每个绘制的框都是有效，不高度重叠且具有一定面积的。有效地减少了图像中的干扰元素，使得红色区域的显示更加清晰。

### 4. 高亮区域提取与图形学处理

- **灰度化和二值化**：将图像灰度化后，使用 `threshold` 进行二值化，提取高亮区域。
- **形态学操作**：使用 `dilate` 和 `erode` 进行膨胀和腐蚀操作，增强特定特征。
- **漫水填充**：使用 `floodFill` 填充指定的区域。

### 5. 图像绘制

- 在 `painting.cpp` 中，使用 `rectangle` 和 `circle` 绘制矩形和圆形，应用高斯模糊模拟阴影光晕效果并且再其上重新叠加矩形/圆形的方式绘出图形的实体。
- **绘制文字**：使用 `putText` 绘制文字，同上思路添加光晕效果。

### 6. 图像旋转与裁剪

- **旋转**：简单地调用 `getRotationMatrix2D` 和 `warpAffine` 对图像旋转 35 度。

- **无损旋转**：通过计算旋转后图像可能需要的最大宽度和高度，并相应调整旋转矩阵的平移量，来实现无损旋转。旋转后的图像尺寸根据角度计算如下：

  - 旋转后图像的宽度： $new\\_width = |W \cdot \cos(\theta)| + |H \cdot \sin(\theta)|$
  - 旋转后图像的高度： $new\\_height = |W \cdot \sin(\theta)| + |H \cdot \cos(\theta)|$

  然后通过调整旋转矩阵中的平移部分，确保图像内容在旋转后居中，避免被裁剪。

- **裁剪**：使用 `Rect` 定义区域，截取原图的左上角 1/4。

## 结果展示

终端运行结果截图：

<img src="resources/terminal_1.png" alt="terminal_1" style="zoom:20%;" />

<img src="resources/terminal_2.png" alt="terminal_2" style="zoom:20%;" />

处理后的图像均已保存至 `../resources/` 目录，包括：

- `gray_image.png`

  <img src="resources/gray_image.png" alt="gray_image" style="zoom:15%;" />

- `hsv_image.png`

  <img src="resources/hsv_image.png" alt="hsv_image" style="zoom:15%;" />

- `mean_blur.png`

  <img src="resources/mean_blur.png" alt="mean_blur" style="zoom:15%;" />

- `gaussian_blur.png`

  <img src="resources/gaussian_blur.png" alt="gaussian_blur" style="zoom:15%;" />

- `red_mask.png`

  <img src="resources/red_mask.png" alt="red_mask" style="zoom:15%;" />

- `highlight_regions.png`

  <img src="resources/highlight_regions.png" alt="highlight_regions" style="zoom:15%;" />

- `contour_image.png`

  <img src="resources/contour_image.png" alt="contour_image" style="zoom:15%;" />

- `bounding_box_image.png`

  <img src="resources/bounding_box_image.png" alt="bounding_box_image" style="zoom:15%;" />

- `dilated_image.png`

  <img src="resources/dilated_image.png" alt="dilated_image" style="zoom:15%;" />

- `eroded_image.png`

  <img src="resources/eroded_image.png" alt="eroded_image" style="zoom:15%;" />

- `filled_image.png`

  <img src="resources/filled_image.png" alt="filled_image" style="zoom:15%;" />

- `rotated_image.png`

  <img src="resources/rotated_image.png" alt="rotated_image" style="zoom:15%;" />

- `rotated_image_lossless.png`

  <img src="resources/rotated_image_lossless.png" alt="rotated_image_lossless" style="zoom:15%;" />

- `cropped_image.png`

  <img src="resources/cropped_image.png" alt="cropped_image" style="zoom:15%;" />

- `rectangle_circle_text.png`

  <img src="resources/rectangle_circle_text.png" alt="rectangle_circle_text" style="zoom: 50%;" />

- `rectangle_circle_text_red.png`

  <img src="resources/rectangle_circle_text_red.png" alt="rectangle_circle_text_red" style="zoom:50%;" />

## 收获

**通过本次任务，我熟悉了 OpenCV 中常用的图像处理操作，包括颜色空间转换、滤波、轮廓检测、形态学操作、图形绘制和图像变换等。学会了如何组合使用这些操作来完成复杂的图像处理任务。同时，对图像中目标的提取和处理有了更深入的理解，为后续更高级的图像处理和计算机视觉任务打下了坚实的基础。**

## 附

### RM Logo 绘制（logo_painting.cpp）

**首先使用 hough_line 直线检测算法检测出 RM Logo 中的线段并获得端点坐标，然后基于坐标连接绘制直线、半圆形成轮廓，然后提取 logo 主体的边界轮廓，并在轮廓内，基于像素点横坐标计算渐变颜色并逐像素点上色形成渐变效果。**

![robomaster_logo](resources/robomaster_logo.png)
