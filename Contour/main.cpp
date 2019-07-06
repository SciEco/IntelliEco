//
//  main.cpp
//  Contour
//
//  Created by william on 2019/7/6.
//  Copyright © 2019 william. All rights reserved.
//

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

//Define global variables
Mat SourceImg;
Mat LineImg;
Mat dst;
Mat DetectedEdges;

int lowThreshold = 0;
const char* window_name = "Edge Map";


int main(void)
{
//Load an image:
    SourceImg = imread( "/Users/william/6.jpeg" );
    if( SourceImg.empty() )
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        return -1;
    }
    cvtColor(SourceImg, SourceImg, CV_BGR2GRAY);
//Initialize the destination image:
    //blur( SourceImg,SourceImg, Size(8,8) );
    
//Edge detection using Canny:
    Canny( SourceImg, DetectedEdges, 100,150 );
    imshow( window_name, DetectedEdges );
    waitKey(0);
    destroyWindow(window_name);
    
//Get the edge of the *white* board:
    vector<vector<Point>> contours;    //Claim the container (2 lines)
    vector<Vec4i> hierarchy;
    
    findContours(DetectedEdges, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);    //Get the edges
    //cout << contours.size();
    
//Find the biggest area:
    vector<vector<Point>> polyContours(contours.size());
    int maxArea = 0;
    for (int index = 0; index < contours.size(); index++){
        if (contourArea(contours[index]) > contourArea(contours[maxArea]))
            maxArea = index;
        approxPolyDP(contours[index], polyContours[index], 10, true);
    }
    
    Mat polyPic = Mat::zeros(SourceImg.size(), CV_8UC3);
    drawContours(polyPic, polyContours, maxArea, Scalar(0,0,255/*rand() & 255, rand() & 255, rand() & 255*/), 2);

    //IMPORTANT!! Must convert Polypic to GRAYSCALE to draw lines by using Houghlines
    //Costs me much time to DE this BUG
    cvtColor(polyPic, polyPic, CV_BGR2GRAY);
    
    /*
    namedWindow("TST3");
    imshow("TST3",polyPic);
    waitKey();      //PolyPic is the Contour
     */

//To Detect lines:
    vector<Vec2f> lines; // will hold the results of the detection
    HoughLines(polyPic, lines,1, CV_PI/180, 200,3,0); // runs the actual detection
    //Polypic: Source image; lines: container of line's parameter(rho,theta); CV/PI/180: precision of theta(rad); 150: precision of rho.
    
//To store lines:
    LineImg.create( SourceImg.size(), SourceImg.type() ); //Seemed useless...But it proofed its value.
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( LineImg, pt1, pt2, Scalar(255,1,1), 3, LINE_AA);
    }

    //cvtColor(LineImg, LineImg, CV_BGR2GRAY);
    addWeighted(LineImg, 0.5, SourceImg, 0.5, 0.7, dst);
    
    namedWindow("Source");
    imshow("Source", dst);
    waitKey();
    
    /*addWeighted(polyPic, 0.5, srcf y, 0.5, 3, det);
    namedWindow("TST4");
    imshow ("TST4",det);
    waitKey();
     */
    return 0;
}
