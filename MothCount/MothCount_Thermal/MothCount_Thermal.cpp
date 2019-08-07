#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <list>
#include <queue>
#include <deque>

#include "MothCount-Thermal-CutImage/CutPicture_Gray.hpp"

using namespace cv;
using namespace std;

bool Counted[2001][2001] = { false };

int dist(Point & p, Point & q)
{
	return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

class Block
{
public:
	Block();
	void add(Point p);
	long area() const;
	Point getCog();
	int getR();
	int getAPR2();
	void erase(Mat & mat);

	deque<Point> pixels;
	bool white;
	Point cog;
	int r;
};

Block::Block()
{
	r = 0;
	cog = Point(-1, -1);
}

void Block::add(Point p)
{
	pixels.push_back(p);
}

long Block::area() const 
{
	return pixels.size();
}

Point Block::getCog()
{
	if (cog != Point(-1, -1)) return cog;
	int xSum = 0, ySum = 0;
	for (Point & p : pixels)
		xSum += p.x, ySum += p.y;
	cog.x = xSum / area(), cog.y = ySum / area();
	return cog;
}

int Block::getR()
{
	if (r) return r;
	getCog();
	for (Point & p : pixels)
		r = max(r, dist(p, cog));
	return r;
}

int Block::getAPR2()
{
	return 100 * area() / max(1, getR()) / max(1, getR());
	// Single moth APR2 falls between 100 and 200
}

void Block::erase(Mat & mat)
{
	for (Point & p : pixels)
		mat.at<uchar>(p.y, p.x) = white ? 0 : 255;
	return;
}

bool operator<(const Block & a, const Block & b)
{
	return a.area() < b.area();
}

//string Filenames[8] = { "IR_01920.jpg", "IR_01921.jpg", "IR_01922.jpg", "IR_01926.jpg", "IR_01931.jpg", "IR_01938.jpg", "IR_01941.jpg", "IR_01944.jpg" };

int main(int argc, char ** argv)
{
	string filename;
	if (argc > 1) filename = argv[0];
	else
	{
		cout << "Enter the path of the image to be tested: ";
		getline(cin, filename);
	}
	Mat image;
	image = imread(filename, IMREAD_GRAYSCALE); // Read the file
	if (image.empty()) // Check for invalid input
	{
		//cout << "Could not open or find the image" << std::endl;
		//cin.get();
		//cin.get();
		return -2;
	}

	//testImage("Source", image);

	// Compress
	while (image.cols >= 2000)
	{
		pyrDown(image, image, Size(image.cols / 2, image.rows / 2));
	}

	CutImage(image, image);
	//testImage("cut", image);

	// If oriented potrait, then transpose it landscape
	if (image.cols < image.rows)
	{
		transpose(image, image);
		flip(image, image, 1);
	}

	// Calculate basic params
	int col = image.cols, row = image.rows;
	int areaSum = col * row;
	auto get = [&image](int x, int y) {return image.at<uchar>(y, x); };

	// Threshold
	//int greySum = 0;
	//for (int i = 0; i < col; i++) for (int j = 0; j < row; j++) greySum += get(i, j);
	//int greyMean = greySum / areaSum;
	//threshold(image, image, greyMean * 0.75, 255, 0);
	/*
	cvtColor(image, image, COLOR_BGR2HSV);
	{
	Mat temp(image.rows, image.cols, CV_8UC1);
	for (int i = 0; i < image.cols; i++) for (int j = 0; j < image.rows; j++)
	{
	temp.at<uchar>(j, i) = get(3 * i, j);
	}
	image = temp;
	}
	testImage("de-colour", image);
	*/

	// Adaptive Threshold
	adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 16);

	//testImage("Thresholded", image);

	// BFS algorithm
	auto bfs = [get, col, row](int x, int y)
	{
		if (Counted[x][y])
			return Block();
		Block block;
		block.white = (get(x, y) == 255 ? true : false);
		queue<Point> q;
		q.emplace(x, y);
		while (!q.empty())
		{
			Point p = q.front();
			q.pop();
			if (Counted[p.x][p.y]) continue;
			Counted[p.x][p.y] = true;
			block.add(p);
			if (p.x - 1 >= 0) if (get(p.x - 1, p.y) == get(x, y)) q.emplace(p.x - 1, p.y);
			if (p.x + 1 < col) if (get(p.x + 1, p.y) == get(x, y)) q.emplace(p.x + 1, p.y);
			if (p.y - 1 >= 0) if (get(p.x, p.y - 1) == get(x, y)) q.emplace(p.x, p.y - 1);
			if (p.y + 1 < row) if (get(p.x, p.y + 1) == get(x, y)) q.emplace(p.x, p.y + 1);
		}
		return block;
	};

	// Block search lambda
	list<Block> block_list;
	auto blockSearch = [&block_list, bfs, &col, &row]()
	{
		block_list.clear();
		memset(Counted, 0, sizeof(Counted));
		for (int i = 0; i < col; i++) for (int j = 0; j < row; j++)
		{
			Block block = bfs(i, j);
			if (block.area() != 0)
				block_list.emplace_back(block);
		}
	};

	// Print initial block info 
	//blockSearch();
	//block_list.sort();
	//for (Block & block : block_list)
	//	cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR2: " << block.getAPR2() << '\n';

	// Dilate
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	dilate(image, image, element);

	//testImage("Dilated", image);

	//GetCountour(_image);

	// Block search 1
	blockSearch();

	// Erase all tiny blocks and small long black blocks
	for (Block & block : block_list)
	{
		if ((block.area() <= 50)
			|| (block.area() <= 7000 && block.getAPR2() <= 50 && !block.white))
		{
			block.erase(image);
		}
	}

	//testImage("Erased", image);

	// Erode
	//element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(3, 3));
	//erode(image, image, element);

	//testImage("Eroded", image);

	// Block search 2
	blockSearch();

	// Erase all tiny blocks and small white blocks
	for (Block & block : block_list)
	{
		if ((block.area() <= 50)
			|| (block.area() <= 7000 && block.white))
		{
			block.erase(image);
		}
	}

	//testImage("Erased", image);

	// Block search 3
	blockSearch();

	// Print block info
	//block_list.sort();
	//for (Block & block : block_list)
	//	cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR2: " << block.getAPR2() << '\n';

	//See if the board exists
	long maximumWhite = 0;
	for (Block & block : block_list)
	{
		if (maximumWhite < block.area() && block.white)
			maximumWhite = block.area();
	}
	if (maximumWhite < 50000) return -1;

	/*
	// Calculate boardArea
	int boardArea = 0;
	for (Block & block : block_list)
	{
	if ((block.area() < 7000 && !block.white) || (block.area() >= 7000 && block.white))
	boardArea += block.area();
	} // TODO: replace 7000 with another variable depending on areaSum


	// Calculate singleMothArea
	int singleMothArea = boardArea * 3.00e-3;
	int minMothArea = singleMothArea / 2;
	*/


	// 632.61 per moth with board area being 427280
	// moth/board = 1.60e-3
	// //cout << mothArea / 65.0 << ' ' << boardArea << '\n';
	int mothArea = 0, approxMothCount = 0;
	for (Block & block : block_list)
		if (!block.white && block.area() < 7000) mothArea += block.area(), approxMothCount++;
	int singleMothArea = mothArea / approxMothCount;

	//cout << "board area: " << boardArea << '\n';
	//cout << "single moth area: " << singleMothArea << '\n';
	//cout << "total moth area: " << mothArea << '\n';

	int mothCount = 0;
	for (Block & block : block_list)
		if (!block.white && block.area() >= singleMothArea / 2) mothCount++;

	cout << mothCount << " moth(s) counted. ";

	cin.get();
	cin.get();

	//testImage("Result", image); // Show our image inside it.

	return 0;
	
}
