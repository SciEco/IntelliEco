//
//  main.cpp
//  MothCount-Thermal
//
//  Created by william on 2019/7/30.
//  Copyright © 2019 william. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>

//#include "CutPicture.hpp"
#include "CutPicture_Gray.hpp"

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    //Read an image
    //freopen("/Users/william/out.txt", "w", stdout);
    string s = "/Users/william/Documents/20190703-“智慧生态”小课题/software/GitRepo/IntelliEco/MothCount/moth/ThermalSrc/Gray/IR_019";
    string append;
    
    cin >> append;
    s+=append;
    append = ".jpg";
    s+=append;
    
    Mat image = imread(s);
    //BGR at default...
    if (image.empty()) {
        cerr << "No image input!" << endl;
        return -2;
    }
    
    testImage("shabby src", image);
    //Portait to horizontal
    if (image.cols < image.rows)
    {
        transpose(image, image);
        flip(image, image, 1);
    }
    
    //image lossy compression
    while (image.cols >= 2000)
    {
        pyrDown(image, image, Size(image.cols / 2, image.rows / 2));
    }
    
//    FindBiggest(image, image);
    
    //line(image, <#Point pt1#>, <#Point pt2#>, <#const Scalar &color#>)
//    Mat line(image);
//
    long beginTime = clock();
    CutImage(image, image);
    long endTime = clock();
    cerr << (endTime - beginTime)/1000  << "ms" << endl;
//    int areaSum = col * row;
//    auto get = [&image](int x, int y) { return image.at<uchar>(y, x); };
//    auto get_TripleChannel = [&image](int x,int y) { return image.at<Vec3b>(y, x); };
    
    
//    cvtColor(image, image, CV_BGR2GRAY);
    
//    Canny(image, line, 65, 95);
    
//    cout << int(get_TripleChannel(21,21)[0]) << " " << int(get_TripleChannel(21,21)[1]) << " " << int(get_TripleChannel(21,21)[2]) << endl;
    
//    testImage("shabby", image, true);
//    testImage("shibby", line);
    testImage("shabby res", image);
    return 0;
}
