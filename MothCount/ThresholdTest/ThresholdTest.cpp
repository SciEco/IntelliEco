#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

Mat frame, frame_HSV, frame_threshold;

static void on_low_H_thresh_trackbar(int, void *)
{
	low_H = min(high_H - 1, low_H);
	setTrackbarPos("Low H", window_detection_name, low_H);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
static void on_high_H_thresh_trackbar(int, void *)
{
	high_H = max(high_H, low_H + 1);
	setTrackbarPos("High H", window_detection_name, high_H);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
static void on_low_S_thresh_trackbar(int, void *)
{
	low_S = min(high_S - 1, low_S);
	setTrackbarPos("Low S", window_detection_name, low_S);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
static void on_high_S_thresh_trackbar(int, void *)
{
	high_S = max(high_S, low_S + 1);
	setTrackbarPos("High S", window_detection_name, high_S);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
static void on_low_V_thresh_trackbar(int, void *)
{
	low_V = min(high_V - 1, low_V);
	setTrackbarPos("Low V", window_detection_name, low_V);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
static void on_high_V_thresh_trackbar(int, void *)
{
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", window_detection_name, high_V);
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_capture_name, frame_threshold);
	imshow(window_detection_name, frame);
}
int main(int argc, char* argv[])
{
	namedWindow(window_capture_name);
	namedWindow(window_detection_name, WINDOW_AUTOSIZE);
	// Trackbars to set thresholds for HSV values
	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);
	
	frame = imread(R"(C:\Users\haora\Documents\visual studio 2015\Projects\IntelliEco\moth\moth5.jpg)", IMREAD_COLOR); // Read the file

	pyrDown(frame, frame, Size(frame.cols / 2, frame.rows / 2));
	pyrDown(frame, frame, Size(frame.cols / 2, frame.rows / 2));

	// Convert from BGR to HSV colorspace
	cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	// Show the frames

	int hSum = 0, hSum1 = 0, sSum = 0, sSum1 = 0, vSum = 0, vSum1 = 0;
	int col = frame_HSV.cols, row = frame_HSV.rows;
	int areaSum = col * row, area1 = 0;
	auto get = [](int x, int y) {return frame_HSV.at<uchar>(y, x); };
	for (int i = 0; i < col * 3; i += 3) for (int j = 0; j < row; j++)
	{
		// if (j == 0) cout << (int)get(i, j) << ' ' << (int)get(i + 1, j) << ' ' << (int)get(i + 2, j) << '\n';
		
		if (get(i, j) >= 20 && get(i, j) <= 50)
		{
			area1++;
			hSum1 += get(i, j);
			sSum1 += get(i + 1, j);
			vSum1 += get(i + 2, j);
		}

		hSum += get(i, j);
		sSum += get(i + 1, j);
		vSum += get(i + 2, j);
	}
	cout << hSum / areaSum << ' ' << sSum / areaSum << ' ' << vSum / areaSum << '\n';
	cout << hSum1 / area1 << ' ' << sSum1 / area1 << ' ' << vSum1 / area1 << '\n';
	
	waitKey();

	return 0;
}
