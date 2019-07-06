#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>
#include <list>
#include <queue>
#include <deque>

using namespace cv;
using namespace std;

bool Counted[2001][2001] = { false };

class Block
{
	public:
		Block();
		void add(Point p);
		int area() const;

		deque<Point> pixels;
		bool white;
};

Block::Block() {}

void Block::add(Point p)
{
	pixels.push_back(p);
}

int Block::area() const
{
	return pixels.size();
}

bool operator< (const Block & a, const Block & b)
{
	return a.area() < b.area();
}

int main()
{
	Mat image, imageHSV, imageThreshold, imageErode;
	image = imread(R"(C:\Users\haora\Documents\visual studio 2015\Projects\IntelliEco\moth\moth8.jpg)", IMREAD_COLOR); // Read the file
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

	// Convert to HSV colourspace
	cvtColor(image, imageHSV, COLOR_BGR2HSV);

	// Threshold
	inRange(imageHSV, Scalar(22, 19, 70), Scalar(35, 255, 255), imageThreshold);

	// Dilate
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(2, 2));
	dilate(imageThreshold, imageErode, element);

	// Search all blocks
	int col = image.cols, row = image.rows;
	auto get = [&imageErode](int x, int y) {return imageErode.at<uchar>(y, x); };
	auto bfs = [&imageErode, get, col, row](int x, int y)
	{
		if (Counted[x][y]) return Block();
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
	list<Block> blocks;
	for (int i = 0; i < col; i++) for (int j = 0; j < row; j++)
	{
		Block block = bfs(i, j);
		if (block.area() != 0) blocks.emplace_back(block);
	}
	blocks.sort();
	for (Block & block : blocks) cout << (block.white ? "white" : "black") << ' ' << block.area() << '\n';

	int boardArea = 0;
	for (Block & block : blocks)
	{
		if ((block.area() < 7000 && !block.white) || (block.area() >= 7000 && block.white)) boardArea += block.area();
	}

	int singleMothArea = boardArea * 1.25e-3;

	// 632.61 per moth with board area being 427280
	// moth/board = 1.60e-3
	// cout << mothArea / 65.0 << ' ' << boardArea << '\n';

	int minMothArea = singleMothArea / 2;
	int mothArea = 0;
	for (Block & block : blocks)
	{
		if (block.area() < 7000 && !block.white && block.area() > minMothArea) mothArea += block.area();
	}

	cout << "board area: " << boardArea << '\n';
	cout << "single moth area: " << singleMothArea << '\n';
	cout << "total moth area: " << mothArea << '\n';
	cout << mothArea / singleMothArea << " moth(s) counted. ";

	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", imageErode); // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}
