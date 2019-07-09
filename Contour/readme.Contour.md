This directory is initialized by w.xu

DrawContour.hpp
This cpp header is meant to provide a function which draw the contour lines of a white board using openCV2.

ACKNOWLEDGEMENTS

NOTE : Jump to statements of "vector<vector<Vec2f> > GetCountour(Mat& InputImage)" if you want to get a quick start.

This hpp contains the following headers
    "opencv2/imgproc.hpp"
    "opencv2/highgui.hpp"
    <iostream>
    <vector>
    <cmath>
using the following namespaces
    cv
    std

Because the contour of those boards in the image are usually not straight lines, we uses 3 lines to describe each edge(up, down, left, right). You can also personalize this setting by changing the line:
    const int LinePerEdge = 3;

Functions:

void testImage(const string WindowName, Mat TestingImage, bool DestroyOrNot = true)
Instead of creating a window on your own, show it, and then adding a waitKey() all by yourself, this function provides a elegant way of showing an image(Mat), from creating a window to destroying it.
Parameters:
    const string WindowName -- Name of the window in the window caption that may be used as a window identifier.
    Mat TestingImage -- The image you want to show.
    bool DestroyOrNot -- true:Destroy the window when the program is over.
    
void FindBiggest(Mat& src, Mat& dst)
This function uses the Canny Edge Detector and function "findContours"
This function is designed to find the contour of the largest white area(the board). Though function "findContours" may find out some small contours not wanted, this function can go through every contour to find the largest one.
Parameters:
    Mat& src -- The source image.
    Mat& dst -- The image to store the largest contour.
    
vector<vector<Vec2f> > DetectAndDrawLines(Mat& LineImage, Mat& DstImage)
This function uses the Hough Line Detector.
This function does the contour-line finding, filtering, drawing and storing job, it will finally give out a vector<vector<Vec2f> >, which contains a 3-layer vector nesting structure.
Parameters:
    Mat& LineImage -- Image only containing contours.(FindBiggest's dst can be directly used.)
    Mat& DstImage -- Image which hold the lines.(Usually the source image)

vector<vector<Vec2f> > GetCountour(Mat& InputImage)
The *main* function of this hpp, give out the rho and theta of the countour lines by returning a vector<vector<Vec2f> >, the 3-layer structure.
Parameters:
    Mat& InputImage -- input the source grayscale image.
Returnings:
    return DetectAndDrawLines(_LineImg, _SourceImg);
    The structure of the 3-layer structure:
        1st layer: Directions.
            [0] -- top/up contours
            [1] -- bottom/down
            [2] -- left
            [3] -- right
        2nd layer: Each of the lines.
            e.g.
            [0][0] -- 1st line on the top
            [3][2] -- 3rd line on the right
        3rd layer: Rho(first position) and Theta(second position) of each line.
            e.g.
            [0][0][0] -- Rho of the 1st line on the top.
            [0][0][1] -- Theta of the 1st line on the top.
