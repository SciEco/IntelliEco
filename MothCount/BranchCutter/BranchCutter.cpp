// [OBSOLETED]
// It's successful, but just TOO SLOW. 

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <deque>
#include <list>
#include <queue>
#include <math.h>

using namespace std;
using namespace cv;

const string window_capture_name = "Video Capture";

bool Counted[2001][2001] = { false };
bool Checked[2001][2001] = { false };

long long int Repetition = 0;

int dist2(Point & p, Point & q)
{
	return (p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y);
}

int taxicab(Point & p, Point & q)
{
	return abs(p.x - q.x) + abs(p.y - q.y);
}

class Block
{
public:
	Block();
	void add(Point p);
	int area() const;
	Point getCog();
	int getR2();
	int areaPerR();
	void fill(Mat & mat);

	deque<Point> pixels;
	bool white;
	Point cog;
	int r2;
};

Block::Block()
{
	r2 = 0;
	cog = Point(-1, -1);
}

void Block::add(Point p)
{
	pixels.push_back(p);
}

int Block::area() const
{
	return pixels.size();
}

Point Block::getCog()
{
	if (cog != Point(-1, -1)) return cog;
	int xSum = 0, ySum = 0;
	for (Point & p : pixels) xSum += p.x, ySum += p.y;
	cog.x = xSum / max(1, area()), cog.y = ySum / max(1, area());
	return cog;
}

int Block::getR2()
{
	if (r2) return r2;
	getCog();
	for (Point & p : pixels) r2 = max(r2, dist2(p, cog));
	return r2;
}

int Block::areaPerR()
{
	return 100 * area() / max(1, getR2());
	// Single moth APR falls between 100 and 200
}

void Block::fill(Mat & mat)
{
	for (Point & p : pixels) mat.at<uchar>(p.y, p.x) = white ? 0 : 255;
	return;
}

int main(int argc, char* argv[])
{
	namedWindow(window_capture_name, WINDOW_AUTOSIZE);
	// Trackbars to set thresholds for HSV values

	Mat image, imageHSV, imageThreshold, imageErode;
	image = imread(R"(C:\Users\haora\Documents\visual studio 2015\Projects\IntelliEco\moth\moth1.jpg)", IMREAD_COLOR); // Read the file
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		cin.get();
		cin.get();
		return -1;
	}

	// Compress
	while (image.cols >= 2000)
	{
		pyrDown(image, image, Size(image.cols / 2, image.rows / 2));
	}

	// Calculate basic params
	int col = image.cols, row = image.rows;
	int areaSum = col * row;
	auto get = [&imageErode](int x, int y) {return imageErode.at<uchar>(y, x); };

	// Convert to HSV colourspace
	cvtColor(image, imageHSV, COLOR_BGR2HSV);

	// Threshold
	int hSum1 = 0, sSum1 = 0, vSum1 = 0;
	int area1 = 0;
	auto get1 = [&imageHSV](int x, int y) {return imageHSV.at<uchar>(y, x); };
	for (int i = 0; i < col * 3; i += 3) for (int j = 0; j < row; j++)
	{
		if (get1(i, j) >= 20 && get1(i, j) <= 50)
		{
			area1++;
			hSum1 += get1(i, j);
			sSum1 += get1(i + 1, j);
			vSum1 += get1(i + 2, j);
		}
	}
	int hMean = hSum1 / area1;
	inRange(imageHSV, Scalar(hMean - 5, 19, 70), Scalar(hMean + 10, 255, 255), imageThreshold);

	auto get2 = [&imageThreshold](int x, int y) -> uchar & {return imageThreshold.at<uchar>(y, x); };
	auto bfs = [get2, col, row](int x, int y, int tax = 2000)
	{
		if (Checked[x][y]) return Block();
		Block block;
		block.white = (get2(x, y) == 255 ? true : false);
		queue<Point> q;
		q.emplace(x, y);
		while (!q.empty())
		{
			Point p = q.front();

			Repetition++;

			q.pop();
			if (Counted[p.x][p.y]) continue;
			Counted[p.x][p.y] = Checked[p.x][p.y] = true;
			block.add(p);
			if (p.x - 1 >= 0 && taxicab(Point(x, y), Point(p.x - 1, p.y)) <= tax) if (get2(p.x - 1, p.y) == get2(x, y)) q.emplace(p.x - 1, p.y);
			if (p.x + 1 < col && taxicab(Point(x, y), Point(p.x + 1, p.y)) <= tax) if (get2(p.x + 1, p.y) == get2(x, y)) q.emplace(p.x + 1, p.y);
			if (p.y - 1 >= 0 && taxicab(Point(x, y), Point(p.x, p.y - 1)) <= tax) if (get2(p.x, p.y - 1) == get2(x, y)) q.emplace(p.x, p.y - 1);
			if (p.y + 1 < row && taxicab(Point(x, y), Point(p.x, p.y + 1)) <= tax) if (get2(p.x, p.y + 1) == get2(x, y)) q.emplace(p.x, p.y + 1);
		}
		return block;
	};

	list<Block> blocks;
	for (int i = 0; i < col; i++) for (int j = 0; j < row; j++)
	{
		memset(Counted, 0, sizeof(Counted));
		Block block = bfs(i, j, 20);
		//block.areaPerR();
		if (block.areaPerR() <= 64)
		{
			block.fill(imageThreshold);
		}
		//if (block.area()) blocks.emplace_back(block);
	}

	//for (Block & block : blocks) cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR: " << block.areaPerR() << '\n';

	
	for (Block & block : blocks)
	{
		if (block.areaPerR() <= 64)
		{
			block.fill(imageThreshold);
		}
	}
	
	imshow(window_capture_name, imageThreshold);

	waitKey();

	return 0;
}
