#include <opencv2/highgui.hpp>
#include <list> //No matter
#include <cmath> //abs()

// A milestone! No Mount. Bullshxt added!

void testImage(const std::string WindowName,\
               cv::Mat TestingImage,\
               bool DestroyOrNot = true)
{
    //Show an image on window "winname", then destroy the window when pressing a key.
    cv::namedWindow(WindowName); //Create an window
    cv::imshow(WindowName, TestingImage); //Show the image on the window
    cv::waitKey(); //Waiting for pressing a key
    
    if (DestroyOrNot)
        cv::destroyWindow(WindowName); //Destroy the window
}

void SingleCut(cv::Mat & ToBeCut,\
            cv::Mat & Destination,\
            int thresholdCol_multiple=15,\
            int thresholdRow_multiple=15,\
            int thresholdCol_mono=50,\
            int thresholdRow_mono=50)
{
    //testImage("shabby 0", ToBeCut);
    std::list<int>memcol;
    std::list<int>memrow;
    int col=ToBeCut.cols, row=ToBeCut.rows;
    
    for (int i_cols = 0 ; i_cols < col-5 ; i_cols+=4)
    {
        double avg_b=0, avg_g=0, avg_r=0;
        //double avg_b_next=0, avg_g_next=0, avg_r_next=0;
        for (int i_rows = 0 ; i_rows < row ; i_rows++)
        {
            avg_b += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[0];
            avg_b -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[0];
            avg_g += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[1];
            avg_g -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[1];
            avg_r += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[2];
            avg_r -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[2];
            
        }
        
        avg_b = std::abs(avg_b / row);
        avg_g = std::abs(avg_g / row);
        avg_r = std::abs(avg_r / row);
        
        //std::cout << avg_b << " " << avg_g << " " << avg_r << std::endl;
        if (avg_b >= thresholdCol_mono || avg_g >= thresholdCol_mono || avg_r >= thresholdCol_mono)
        {
            memcol.push_back(i_cols);
            //std::cout << avg_b << "\t" << avg_g << "\t" << avg_r << "\tcolumn:" << i_cols << std::endl;
            //cv::line(ToBeCut, cv::Point(i_cols,0), cv::Point(i_cols,row), cvScalar(255,255,255), 5, cv::LINE_8, 0);
            i_cols += 7;
        }
        else if ((avg_b >=thresholdCol_multiple && avg_g >= thresholdCol_multiple) ||\
                (avg_g >= thresholdCol_multiple && avg_r >= thresholdCol_multiple) ||\
                (avg_b >= thresholdCol_multiple && avg_r >= thresholdCol_multiple  ))
        {
            memcol.push_back(i_cols);
            //std::cout << avg_b << "\t" << avg_g << "\t" << avg_r << "\tcolumn:" << i_cols << std::endl;
            //cv::line(ToBeCut, cv::Point(i_cols,0), cv::Point(i_cols,row), cvScalar(255,255,255), 5, cv::LINE_8, 0);
            i_cols += 7;
        }
        
    }
    
    for (int i_rows = 0 ; i_rows < row-5 ; i_rows+=4)
    {
        double avg_b=0, avg_g=0, avg_r=0;
        //double avg_b_next=0, avg_g_next=0, avg_r_next=0;
        for (int i_cols = 0 ; i_cols < col ; i_cols++)
        {
            avg_b += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[0];
            avg_b -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[0];
            avg_g += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[1];
            avg_g -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[1];
            avg_r += ToBeCut.at<cv::Vec3b>(i_rows, i_cols)[2];
            avg_r -= ToBeCut.at<cv::Vec3b>(i_rows, i_cols+4)[2];
            
        }
        
        avg_b = std::abs(avg_b / col);
        avg_g = std::abs(avg_g / col);
        avg_r = std::abs(avg_r / col);
        
        //std::cout << avg_b << " " << avg_g << " " << avg_r << std::endl;
        if (avg_b >= thresholdRow_mono || avg_g >= thresholdRow_mono || avg_r >= thresholdRow_mono)
         {
             memrow.push_back(i_rows);
             //std::cout << avg_b << "\t" << avg_g << "\t" << avg_r << "\trow:" << i_rows << std::endl;
             //cv::line(ToBeCut, cv::Point(i_rows,0), cv::Point(i_rows,row), cvScalar(255,255,255), 5, cv::LINE_8, 0);
         }
         else if ((avg_b >=thresholdRow_multiple && avg_g >= thresholdRow_multiple) ||\
                    (avg_g >= thresholdRow_multiple && avg_r >= thresholdRow_multiple) ||\
                    (avg_b >= thresholdRow_multiple && avg_r >= thresholdRow_multiple))
         {
             memrow.push_back(i_rows);
             //std::cout << avg_b << "\t" << avg_g << "\t" << avg_r << "\trow:" << i_rows << std::endl;
             //cv::line(ToBeCut, cv::Point(i_rows,0), cv::Point(i_rows,row), cvScalar(255,255,255), 5, cv::LINE_8, 0);
         }
        
    }
//    testImage("shabby", ToBeCut);
    
    //See if 0 or "col"/"row" is included
    if (memcol.size()>=2)
    {
        if (memcol.front() == 0)
            memcol.pop_front();
        if (memcol.back() == col)
            memcol.pop_back();
    }
    
    if (memrow.size()>=2)
    {
        if (memrow.front() == 0)
            memrow.pop_front();
        if (memrow.back() == row)
            memrow.pop_back();
    }
    
    //See if found two egdes
    if (memcol.empty())
    {
        memcol.push_back(0);
        memcol.push_back(col);
    }
    else if (memcol.size() == 1)
    {
        memcol.pop_back();
        memcol.push_back(0);
        memcol.push_back(col);
    }

    if (memrow.empty())
    {
        memrow.push_back(0);
        memrow.push_back(row);
    }
    else if (memrow.size() == 1)
    {
        memrow.pop_back();
        memrow.push_back(0);
        memrow.push_back(col);
    }

    cv::Rect m_select = cv::Rect(memcol.front(), memrow.front(), memcol.back()-memcol.front(), memrow.back()-memrow.front());

    //testImage("shabby 0", ToBeCut);

    Destination = ToBeCut(m_select);

//    testImage("shabby", Destination);
//    std::cout << std::endl;
}

void CutImage(const cv::Mat & Source, cv::Mat & Destination)
{
    Destination = Source;
    // Define controlling variables
    bool colContinue = true, rowContinue = true;
    int CutCount = 0;
    // Basic metadata of the image
    int col = Destination.cols, row = Destination.rows;
    int lastCol = col, lastRow = row;
    // Base Threshold definition
    int mulRow=15,monRow=50, mulCol=15, monCol=50;
    // Hold the result of last cutting
    // To recover when a ~~failed~~ cut occurs
    cv::Mat Lastime(Destination);
    // Do the cutting job
    // Stop when cut to 3/4 size
    while ((Destination.cols >= col * 3.0 / 5 && Destination.rows >= row * 3.0 / 5)\
           &&\
           (colContinue || rowContinue)\
           &&\
           (CutCount <= 30))
    {
        // Do a single cutting
        SingleCut(Destination, Destination, mulCol, mulRow, monCol, monRow);
        
        // To detect a bad cut
        if (Lastime.cols >= Destination.cols * 2)
        {
            Destination = Lastime;
            colContinue = false;
            mulCol++;
            monCol++;
        }
        if (Lastime.rows >= Destination.rows * 2)
        {
            Destination = Lastime;
            rowContinue = false;
            mulRow++;
            monRow++;
        }
        
        // When no cutting is done, reduce the threshold
        if (colContinue)
        {
            if (lastCol == Destination.cols)
            {
                mulCol--;
                monCol--;
            }
            else;
        }
        
        if (rowContinue)
        {
            if (lastRow == Destination.rows)
            {
                mulRow--;
                monRow--;
            }
            else;
        }
        
        // Update Last time
        Lastime = Destination;
        lastCol = Destination.cols;
        lastRow = Destination.rows;
        CutCount++;
    }
}
