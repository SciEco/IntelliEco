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

int dist(Point & p, Point & q)
{
	return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

class Block
{
public:
	Block();
	void add(Point p);
	int area() const;
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

int Block::area() const
{
	return pixels.size();
}

Point Block::getCog()
{
	if (cog != Point(-1, -1)) return cog;
	int xSum = 0, ySum = 0;
	for (Point & p : pixels) xSum += p.x, ySum += p.y;
	cog.x = xSum / area(), cog.y = ySum / area();
	return cog;
}

int Block::getR()
{
	if (r) return r;
	getCog();
	for (Point & p : pixels) r = max(r, dist(p, cog));
	return r;
}

int Block::getAPR2()
{
	return 100 * area() / max(1, getR()) / max(1, getR());
	// Single moth APR2 falls between 100 and 200
}

void Block::erase(Mat & mat)
{
	for (Point & p : pixels) mat.at<uchar>(p.y, p.x) = white ? 0 : 255;
	return;
}

bool operator<(const Block & a, const Block & b)
{
	return a.area() < b.area();
}

int main()
{
	Mat image, imageHSV, imageThreshold, imageErode;
	image = imread(R"(C:\Users\haora\Documents\visual studio 2015\Projects\IntelliEco\moth\moth4.jpg)", IMREAD_COLOR); // Read the file
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

	// If oriented potrait, then transpose it landscape
	if (col < row)
	{
		transpose(image, image);
		swap(col, row);
	}

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

	// Block search 1
	auto get2 = [&imageThreshold](int x, int y) {return imageThreshold.at<uchar>(y, x); };
	auto bfs = [get2, col, row](int x, int y)
	{
		if (Counted[x][y]) return Block();
		Block block;
		block.white = (get2(x, y) == 255 ? true : false);
		queue<Point> q;
		q.emplace(x, y);
		while (!q.empty())
		{
			Point p = q.front();
			q.pop();
			if (Counted[p.x][p.y]) continue;
			Counted[p.x][p.y] = true;
			block.add(p);
			if (p.x - 1 >= 0) if (get2(p.x - 1, p.y) == get2(x, y)) q.emplace(p.x - 1, p.y);
			if (p.x + 1 < col) if (get2(p.x + 1, p.y) == get2(x, y)) q.emplace(p.x + 1, p.y);
			if (p.y - 1 >= 0) if (get2(p.x, p.y - 1) == get2(x, y)) q.emplace(p.x, p.y - 1);
			if (p.y + 1 < row) if (get2(p.x, p.y + 1) == get2(x, y)) q.emplace(p.x, p.y + 1);
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

	// Optput block info
	for (Block & block : blocks) cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR2: " << block.getAPR2() << '\n';
	
	// Calculate boardArea
	int boardArea = 0;
	for (Block & block : blocks)
	{
		if ((block.area() < 7000 && !block.white) || (block.area() >= 7000 && block.white)) boardArea += block.area();
	} // TODO: replace 7000 with another variable depending on areaSum

	// Calculate singleMothArea
	int singleMothArea = boardArea * 1.50e-3;
	int minMothArea = singleMothArea / 2;

	// Erase bad blocks and the black frame
	for (Block & block : blocks)
	{
		if ((block.area() <= minMothArea && !block.white)
			|| (block.area() <= 7000 && block.getAPR2() <= 100 && !block.white)
			|| (block.area() >= 7000 && !block.white))
		{
			block.erase(imageThreshold);
		}
	}

	// Dilate
	//Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(2, 2));
	//dilate(imageThreshold, imageErode, element);

	// Block search 2
	blocks.clear();
	memset(Counted, 0, sizeof(Counted));
	for (int i = 0; i < col; i++) for (int j = 0; j < row; j++)
	{
		Block block = bfs(i, j);
		if (block.area() != 0) blocks.emplace_back(block);
	}


	// 632.61 per moth with board area being 427280
	// moth/board = 1.60e-3
	// cout << mothArea / 65.0 << ' ' << boardArea << '\n';
	int mothArea = 0;
	for (Block & block : blocks) if (!block.white) mothArea += block.area();

	cout << "board area: " << boardArea << '\n';
	cout << "single moth area: " << singleMothArea << '\n';
	cout << "total moth area: " << mothArea << '\n';
	cout << mothArea / singleMothArea << " moth(s) counted. ";

	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", imageThreshold); // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}
