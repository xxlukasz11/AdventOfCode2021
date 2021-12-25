#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <vector>
#include <map>
#include <set>

using DataType = std::vector<std::vector<char>>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		auto row = common::parseArray<char>(line);
		data.push_back(row);
	}
	return data;
}

bool moveRightMovers(DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();

	std::vector<std::pair<int, int>> cucumbersToMove;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			if (data[row][col] == '>') {
				if (col < cols - 1 && data[row][col + 1] == '.') {
					cucumbersToMove.emplace_back(row, col);
				}
				else if (col == cols - 1 && data[row][0] == '.') {
					cucumbersToMove.emplace_back(row, col);
				}
			}
		}
	}
	for (const auto& [row, col] : cucumbersToMove) {
		if (col == cols - 1) {
			data[row][0] = '>';
			data[row][col] = '.';
		}
		else {
			data[row][col + 1] = '>';
			data[row][col] = '.';
		}
	}
	return !cucumbersToMove.empty();
}

bool moveDownMovers(DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();
	
	std::vector<std::pair<int, int>> cucumbersToMove;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			if (data[row][col] == 'v') {
				if (row < rows - 1 && data[row + 1][col] == '.') {
					cucumbersToMove.emplace_back(row, col);
				}
				else if (row == rows - 1 && data[0][col] == '.') {
					cucumbersToMove.emplace_back(row, col);
				}
			}
		}
	}
	for (const auto& [row, col] : cucumbersToMove) {
		if (row == rows - 1) {
			data[0][col] = 'v';
			data[row][col] = '.';
		}
		else {
			data[row + 1][col] = 'v';
			data[row][col] = '.';
		}
	}
	return !cucumbersToMove.empty();
}

int partOne(DataType data) {
	int step = 1;
	while (true) {
		auto movedRight = moveRightMovers(data);
		auto movedDown = moveDownMovers(data);
		if (!movedRight && !movedDown) {
			break;
		}
		++step;
	}
	return step;
}

int partTwo(const DataType& data) {

	return 0;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}