#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <utility>
#include <set>

using DataType = std::vector<std::vector<int>>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::istringstream ss(line);
		std::vector<int> row;
		for (char c; ss >> c;) {
			row.push_back(c - '0');
		}
		data.push_back(row);
	}
	return data;
}

bool isLowpoint(int r, int c, const DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();
	const int value = data[r][c];
	if (r - 1 >= 0 && data[r - 1][c] <= value) {
		return false;
	}
	if (r + 1 < rows && data[r + 1][c] <= value) {
		return false;
	}
	if (c + 1 < cols && data[r][c + 1] <= value) {
		return false;
	}
	if (c - 1 >= 0 && data[r][c - 1] <= value) {
		return false;
	}
	return true;
}

struct Dir {
	int r;
	int c;
};

static Dir UP{ -1, 0 };
static Dir DOWN{ 1, 0 };
static Dir LEFT{ 0, -1 };
static Dir RIGHT{ 0, 1 };

Dir rightDir(Dir dir) {
	if (dir.c != 0) {
		return Dir{ dir.c , 0 };
	}
	return Dir{ 0 , -dir.r };
}

Dir leftDir(Dir dir) {
	if (dir.c != 0) {
		return Dir{ -dir.c , 0 };
	}
	return Dir{ 0 , dir.r };
}

bool notInBasin(const std::set<std::pair<int, int>>& basin, int r, int c) {
	return basin.find({ r, c }) == basin.end();
}

void crawl(std::set<std::pair<int, int>>& basin, const DataType& data, int srcR, int srcC, Dir dir) {
	int r = srcR + dir.r;
	int c = srcC + dir.c;
	const int rows = data.size();
	const int cols = data[0].size();
	if (r < 0 || c < 0 || r > rows - 1 || c > cols - 1) {
		return;
	}
	if (!notInBasin(basin, r, c)) {
		return;
	}

	auto value = data[r][c];
	if (value == 9) {
		return;
	}

	basin.emplace(r, c);
	Dir right = rightDir(dir);
	Dir left = leftDir(dir);
	crawl(basin, data, r, c, dir);
	crawl(basin, data, r, c, right);
	crawl(basin, data, r, c, left);
}

int findBasin(int r, int c, const DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();
	std::set<std::pair<int, int>> basin;
	basin.emplace(r, c);
	if (r - 1 >= 0) {
		crawl(basin, data, r, c, UP);
	}
	if (r + 1 < rows) {
		crawl(basin, data, r, c, DOWN);
	}
	if (c + 1 < cols) {
		crawl(basin, data, r, c, RIGHT);
	}
	if (c - 1 >= 0) {
		crawl(basin, data, r, c, LEFT);
	}
	return basin.size();
}

void partOne(const DataType& data) {
	int rows = data.size();
	int cols = data[0].size();
	std::vector<int> lowPoints;
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			if (isLowpoint(r, c, data)) {
				lowPoints.push_back(data[r][c]);
			}
		}
	}

	int sum = std::accumulate(lowPoints.begin(), lowPoints.end(), 0) + lowPoints.size();
	std::cout << "Part one: " << sum << std::endl;
}

void partTwo(const DataType& data) {
	int rows = data.size();
	int cols = data[0].size();
	std::vector<int> basins;
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			if (isLowpoint(r, c, data)) {
				int basinSize = findBasin(r, c, data);
				basins.push_back(basinSize);
			}
		}
	}
	std::sort(basins.begin(), basins.end());
	int basinsSize = basins.size();
	int mul = basins[basinsSize-1] * basins[basinsSize-2] * basins[basinsSize-3];
	std::cout << "Part two: " << mul << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}