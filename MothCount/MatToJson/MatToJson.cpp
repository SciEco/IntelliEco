#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include <deque>

#include "DrawContour.hpp"
#include "json.hpp"

using namespace cv;
using namespace nlohmann;
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
	image = imread(R"(C:\Users\haora\Documents\Visual Studio 2015\Projects\IntelliEco\moth\moth1.2.jpg)", IMREAD_GRAYSCALE); // Read the file

	// Calculate basic params
	int col = image.cols, row = image.rows;
	int areaSum = col * row;
	auto get = [&image](int x, int y) -> uchar & {return image.at<uchar>(y, x); };

	for (int i = 0; i < col; i++) for (int j = 0; j < row; j++) get(i, j) = get(i, j) >= 128 ? 255 : 0;

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

	ofstream ofs("bitmap.json");
	json js;

	js["col"] = col;
	js["row"] = row;
	for (int i = 0; i < col; i++) for (int j = 0; j < row; j++) js["pixels"].push_back(get(i, j));

	ofs << js;
	ofs.close();

	// Block search 
	blockSearch();

	testImage("Window", image);

	// Print block info
	block_list.sort();
	for (Block & block : block_list) if (block.area() >= 10)
		cout << (block.white ? "white" : "black") << "\tarea: " << block.area() << "\tCoG: " << block.getCog().x << ' ' << block.getCog().y << "\tradius: " << block.getR() << "\tAPR2: " << block.getAPR2() << '\n';

	system("pause");

	return 0;
}
