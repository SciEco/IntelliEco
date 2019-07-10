#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <list>
#include <queue>
#include <deque>

#include "DrawContour.hpp"

using namespace cv;
using namespace std;

bool Counted[2001][2001] = { false };

//william-NewDefined
vector<vector<Vec2f> > ContourLines;

bool IfLower(double rho, double theta, Point p)
//1 - Bigger/Lower than y = kx + b
//0 - Smaller/Higher than y = kx + b
{
    double k = -1 * cos(theta)/sin(theta), b = rho/sin(theta);
    double yShouldBe = k * p.x + b;
    //cout << yShouldBe;
    if (yShouldBe <= double(p.y))
        return true;
    else
        return false;
}

bool IfRighter(double rho, double theta, Point p)
//1 - Righter than y = mx + n
//0 - Lefter than y = mx + n
{
    double m = -1 * tan(theta), n = -1 * rho/sin(theta) / tan(theta);
    double xShouldBe = m * p.y + n;
    if (xShouldBe <= p.x)
        return true;
    else
        return false;
}
//END William-NewDefined

int dist(Point & p, Point & q)
{
	return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

class Block
{
public:
	Block();
	void add(Point p);
	long area() const; //
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

long Block::area() const //
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

int main(int argc, char ** argv)
{
	Mat image;
    Mat &_image = image;
	image = imread(argv[1], IMREAD_COLOR); // Read the file
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		cin.get();
		cin.get();
		return -1;
	}
    
    testImage("Source", image);

	// If oriented potrait, then transpose it landscape
	if (image.cols < image.rows)
	{
		transpose(image, image);
		flip(image, image, 1);
	}

	// Compress
	while (image.cols >= 2000)
	{
		pyrDown(image, image, Size(image.cols / 2, image.rows / 2));
	}

	// Calculate basic params
	int col = image.cols, row = image.rows;
	int areaSum = col * row;
	auto get = [&image](int x, int y) {return image.at<uchar>(y, x); };

	// Convert to HSV colourspace
	cvtColor(image, image, COLOR_BGR2HSV);

	// Threshold
	int hSum1 = 0, sSum1 = 0, vSum1 = 0;
	int area1 = 0;
	for (int i = 0; i < col * 3; i += 3) for (int j = 0; j < row; j++)
	{
		if (get(i, j) >= 20 && get(i, j) <= 50)
		{
			area1++;
			hSum1 += get(i, j);
			sSum1 += get(i + 1, j);
			vSum1 += get(i + 2, j);
		}
	}
	int hMean = hSum1 / area1;
	inRange(image, Scalar(hMean - 5, 19, 70), Scalar(hMean + 10, 255, 255), image);

    testImage("Threshold", image);

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
    
	// Dilate
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
	dilate(image, image, element);

     GetCountour(_image);
    testImage("Dilated", image);
    
	// Block search 1
	blockSearch();

	// Erase all tiny blocks and small long black blocks
	for (Block & block : block_list)
	{
		if ((block.area() <= 250)
			|| (block.area() <= 7000 && block.getAPR2() <= 50 && !block.white))
		{
			block.erase(image);
		}
	}

	testImage("Erased", image);

	// Erode
	element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(3, 3));
	erode(image, image, element);

	testImage("Eroded", image);

	// Block search 2
	blockSearch();

	// Print block info
	block_list.sort();
	for (Block & block : block_list)
        cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR2: " << block.getAPR2() << '\n';

	// Erase all tiny blocks and small white blocks
	for (Block & block : block_list)
	{
		if ((block.area() <= 250)
			|| (block.area() <= 7000 && block.white))
		{
			block.erase(image);
		}
	}

	testImage("Erased the second time", image);
    
    testImage("Drawline", image);
    

	// Block search 3
	blockSearch();

	// Calculate boardArea
	int boardArea = 0;
	for (Block & block : block_list)
	{
		if ((block.area() < 7000 && !block.white) || (block.area() >= 7000 && block.white))
            boardArea += block.area();
	} // TODO: replace 7000 with another variable depending on areaSum

	// Calculate singleMothArea
	int singleMothArea = boardArea * 2.30e-3;
	int minMothArea = singleMothArea / 2;


	// 632.61 per moth with board area being 427280
	// moth/board = 1.60e-3
	// cout << mothArea / 65.0 << ' ' << boardArea << '\n';
	int mothArea = 0;
	for (Block & block : block_list)
        if (!block.white && block.area() < 7000) mothArea += block.area();

	cout << "board area: " << boardArea << '\n';
	cout << "single moth area: " << singleMothArea << '\n';
	cout << "total moth area: " << mothArea << '\n';
	cout << mothArea / singleMothArea << " moth(s) counted. ";

    testImage("Result", image); // Show our image inside it.
    
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}
