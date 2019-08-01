#include <opencv2/highgui.hpp>
#include <vector> //No matter
#include <cmath> //abs()

const int LinePerEdge = 2;

//Not defining global variables

void testImage(const std::string WindowName, cv::Mat TestingImage, bool DestroyOrNot = true)
{
    //Show an image on window "winname", then destroy the window when pressing a key.
    cv::namedWindow(WindowName); //Create an window
    cv::imshow(WindowName, TestingImage); //Show the image on the window
    cv::waitKey(); //Waiting for pressing a key
    
    if (DestroyOrNot)
        cv::destroyWindow(WindowName); //Destroy the window
}

void FindBiggest(cv::Mat& src, cv::Mat& dst) //dst stores the countour lines
{
    cv::Mat dst_middle;
    
    cv::Canny( src, dst_middle, 100,150,5); //Edge detection using Canny:
    
    //Dilate : The "Canny" function draws a rather thin line.
    //So dilate the lines to allow the "findContours" function to find it.
    //Much time to DE this BUG
    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(1, 1));
    cv::dilate(dst_middle, dst_middle, element);
    
    //Get and store the edge of the "white" board:
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy; //The containers
    
    findContours(dst_middle, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); //Get the edges
    
    //Find the biggest area then output it into dst:
    std::vector<std::vector<cv::Point> > polyContours(contours.size());
    int maxArea = 0;
    for (int index = 0; index < contours.size(); index++)
    {
        if (contourArea(contours[index]) > contourArea(contours[maxArea]))
            maxArea = index;
        approxPolyDP(contours[index], polyContours[index], 10, true);
        //cout << contourArea(contours[index]) << endl;
    }
    
    //cout << endl << contourArea(contours[maxArea]) << endl; //Debugging command : print the size of the max area
    
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
    drawContours(dst, polyContours, maxArea, cv::Scalar(0,0,255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
}

void DetectAndDrawLines(cv::Mat& LineImage, cv::Mat& DstImage)
{
    //To Detect lines:
    std::vector<cv::Vec2f> lines; //Container which will hold the results of the 1st-time detection
    cv::HoughLines(LineImage, lines, 1, CV_PI/180, 150,0 ,0); //Runs the actual detection
    //USAGE : dst1: Source image; lines: container of line's parameter(rho,theta); 1: precision of rho; CV/PI/180: precision of theta(rad).
    
    //To draw lines:
    
    int upCount=0, downCount=0, leftCount=0, rightCount=0;
    ////cout << LineImage.cols << " " << LineImage.rows << "\n" << endl;
    
    //Go through all the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        
        //"const int LinePerEdge" lines each edge:
        //Controls the maximum number of lines on each edge.
        if (rho < 0)
        {
            if (rightCount >= LinePerEdge)
                continue;
            else
            {
                rightCount++;
                //cout << "right" << endl;
            }
            
        }
        else if (rho >= 500)
        {
            if (downCount >= LinePerEdge)
                continue;
            else
            {
                downCount++;
                //cout << "down" << endl;
            }
        }
        else
        {
            if (theta >= 0.70)
            {
                if (upCount >= LinePerEdge)
                    continue;
                else
                {
                    upCount++;
                    //cout << "up" << endl;
                }
            }
            else
            {
                if (leftCount >= LinePerEdge)
                    continue;
                else
                {
                    leftCount++;
                    //cout << "left" << endl;
                }
            }
            
        }

        
        cv::Point pt1, pt2; //Using pt1 and pt2 as terminals to draw a segment.
        
        if (theta < 0.001 && theta > -0.001) //If the line is close to vertical
        {
            //double k = -1 * cos(theta)/sin(theta), b = rho/sin(theta);
            double n = -1 * rho/sin(theta) / tan(theta);
            pt1.x = n;
            pt2.x = n;
            pt1.y = 20;
            pt2.y = LineImage.rows - 20;
        }
        else
        {
            double k = -1 * cos(theta)/sin(theta), b = rho/sin(theta);
            //double m = -1 * tan(theta), n = -1 * rho/sin(theta) / tan(theta);
            
            for (int x0=0 ; x0<LineImage.cols ; x0++)
            {
                double y0 = k * x0 + b;
                if (y0 < LineImage.rows && y0 > 0)
                {
                    pt1.x = x0 + 40 * sin(theta);
                    if (k>0)
                        pt1.y = y0 + 40 * abs(cos(theta));
                    else
                        pt1.y = y0 - 40 * abs(cos(theta));
                    break;
                }
            }

            for (int x0 = pt1.x + 1 ; x0<LineImage.cols ; x0++)
            {
                double y0 = k * x0 + b;
                pt2.x = x0 - 40 * sin(theta);
                if (k>0)
                    pt2.y = y0 - 40 * abs(cos(theta));
                else
                    pt2.y = y0 + 40 * abs(cos(theta));
                if (y0 > LineImage.rows || y0 < 0)
                    break;
            }
        }
        
        cv::line(DstImage, pt1, pt2, 255, 5, cv::LINE_8, 0);
    }
}

void GetCountour(cv::Mat& InputImage)
{
    //Define variables
    cv::Mat SourceImg, LineImg;
    cv::Mat& _SourceImg = SourceImg;
    cv::Mat& _LineImg = LineImg;
    
    //Load an image:
    SourceImg = InputImage;
    
    FindBiggest(_SourceImg, _LineImg);
    //IMPORTANT!! Must convert Polypic to GRAYSCALE to (draw lines by using Houghlines)
    //Takes me much time to DE this BUG
    cv::cvtColor(LineImg, LineImg, CV_BGR2GRAY);
    DetectAndDrawLines(_LineImg, InputImage);
}

