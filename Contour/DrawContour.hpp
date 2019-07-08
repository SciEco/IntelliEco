//
//  main.cpp
//  DrawContour
//
//  Created by william on 2019/7/6.
//  Copyright © 2019 W-Hsu. All rights reserved.
//
/*----------------------------------------------*/
//WARNING : USED GLOBAL VARIABLE NAMES (On line 22) :
//SourceImg, LineImg, dstLineImg, _SourceImg, _LineImg, _dstLineImg, DetectedEdges.

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

//Define global variables
Mat SourceImg, LineImg, dstLineImg;
Mat& _SourceImg = SourceImg;
Mat& _LineImg = LineImg;
Mat& _dstLineImg = dstLineImg;
Mat DetectedEdges;

void testImage(const string WindowName, Mat TestingImage, bool DestroyOrNot = true)
{
//Show an image on window "winname", then destroy the window when pressing a key.
    namedWindow(WindowName); //Create an window
    imshow(WindowName, TestingImage); //Show the image on the window
    waitKey(); //Waiting for pressing a key
    
    if (DestroyOrNot)
        destroyWindow(WindowName); //Destroy the window
}

void FindBiggest(Mat& src, Mat& dst)
{
    Mat dst_middle;

//Edge detection using Canny:
    Canny( src, dst_middle, 100,150 );
    
//Get the edge of the "white" board:
    vector<vector<Point> > contours;    //Claim the container (2 lines)
    vector<Vec4i> hierarchy;
    
    findContours(dst_middle, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);    //Get the edges
    
//Find the biggest area then output it into dst:
    vector<vector<Point> > polyContours(contours.size());
    int maxArea = 0;
    for (int index = 0; index < contours.size(); index++){
        if (contourArea(contours[index]) > contourArea(contours[maxArea]))
            maxArea = index;
        approxPolyDP(contours[index], polyContours[index], 10, true);
    }
    
    dst = Mat::zeros(SourceImg.size(), CV_8UC3);
    drawContours(dst, polyContours, maxArea, Scalar(0,0,255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
}

void DetectAndDrawLines(Mat& LineImage, Mat& DstImage)
{
//To Detect lines:
    vector<Vec2f> lines; // will hold the results of the 1st-time detection
    vector<Vec2f> final_lines; // will hold the results of the detection
    HoughLines(LineImage, lines, 1, CV_PI/180, 150,0 ,0); // runs the actual detection
    //dst1: Source image; lines: container of line's parameter(rho,theta); 1: precision of rho; CV/PI/180: precision of theta(rad).

//To draw lines:
    //Memory of the "too close" lines
    float memrho = lines[0][0], memtheta = lines[0][1];
    const double THRESHOLD_RHO = 1000, THRESHOLD_THETA = 10 * CV_PI/180;
    int direction = 0;
    
    //Go through all the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];

/*MARK 1 HERE 2019-07-08*/
    //See if the line is "too close". If so, omit it. If not, remember it.
        if (abs(memrho - rho)<THRESHOLD_RHO || abs(memtheta - theta)<THRESHOLD_THETA)
            continue;
        else
        {
            memrho = rho;
            memtheta = theta;
            direction++;
        }
/*END MARK 1*/
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 2000*(-b));
        pt1.y = cvRound(y0 + 2000*(a));
        pt2.x = cvRound(x0 - 2000*(-b));
        pt2.y = cvRound(y0 - 2000*(a));
        line( DstImage, pt1, pt2, 255, 1, LINE_8,0);
    }
}

int DrawContour()
{
//Load an image:
    SourceImg = imread( "/Users/william/6.jpeg" );
    if( SourceImg.empty() )
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return -1;
    }
    cvtColor(SourceImg, SourceImg, CV_BGR2GRAY);
    inRange(SourceImg, 255, 255, dstLineImg);
    
    
    FindBiggest(_SourceImg, _LineImg);
    
    //IMPORTANT!! Must convert Polypic to GRAYSCALE to (draw lines by using Houghlines)
    //Takes me much time to DE this BUG
    cvtColor(LineImg, LineImg, CV_BGR2GRAY);
    
    DetectAndDrawLines(_LineImg, SourceImg);
    
    testImage("TST1", SourceImg, true);
    
    return 0;
}
