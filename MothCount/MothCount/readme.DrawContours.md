**This directory is initialized by w.xu**

# DrawContour.hpp

## 致谢
**提供机器视觉库与教程**  
[openCV](https://opencv.org)  
**提供"FindBiggest"函数的算法**  
[才才才](https://segmentfault.com/a/1190000013925648?utm_source=tag-newest)  

提示：若需快速上手，请转到关于`void GetCountour(Mat& InputImage)`的说明。

## 功能概述
### 头文件与命名空间：
DrawContours.hpp包含以下头文件：  
```c++
"opencv2/imgproc.hpp"
"opencv2/highgui.hpp"
<iostream>
<vector>
<cmath>
```
### DrawContours.hpp使用命名空间：  
```
cv
std
```

### 函数
本hpp头文件提供如下函数：  
1. `void testImage(const string WindowName, Mat TestingImage, bool DestroyOrNot = true)`  
    此函数提供优雅而简单地查看Mat类图片的功能。该函数基于[openCV2](https://github.com/opencv/opencv/tree/2.4)。    
    参数：
```c++
    const string WindowName
    //指定窗口名称
    Mat TestingImage
    //指定要展示的图片
    bool DestroyOrNot
    //展示结束后是否关闭窗口，默认为是
```

2. `void GetCountour(Mat& InputImage)`
    用以在给定图片中的白色板上画出边界线的函数。该函数基于[openCV2](https://github.com/opencv/opencv/tree/2.4)。 
    参数：
```c++
    Mat& InputImage
    //指定需要画线的图片
```
   由于边界通常并非标准的矩形，本函数默认在每个边界（上、下、左、右）各画2条线以提高精准度，如有需要可以通过修改`const int LinePerEdge = 2;`来自定义这项设置。

3. 不建议使用~~`void FindBiggest(Mat& src, Mat& dst)`~~与~~`void DetectAndDrawLines(Mat& LineImage, Mat& DstImage)`~~两个函数，只提供英文说明。

I. `void FindBiggest(Mat& src, Mat& dst)`
    This function uses the Canny Edge Detector and function "findContours"
    This function is designed to find the contour of the largest white area(the board). Though function "findContours" may find out some small contours not wanted, this function can go through every contour to find the largest one.  
    This function is based on [openCV2](https://github.com/opencv/opencv/tree/2.4)
    Parameters:
```c++
    Mat& src //The source image.
    Mat& dst //The image to store the largest contour.
```

II. `void DetectAndDrawLines(Mat& LineImage, Mat& DstImage)`
    This function uses the Hough Line Detector.
    This function does the contour-line finding, filtering, drawing and storing job, it will finally draw the found lines on DstImage.
    This function is based on [openCV2](https://github.com/opencv/opencv/tree/2.4)
    Parameters:

```c++
    Mat& LineImage //Image only containing contours.(FindBiggest's dst can be directly used.)
    Mat& DstImage //Image which hold the lines.(Usually the source image)
```

