#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <map>
#include <set>
#include <cstdint>

using DataType = std::vector<std::vector<int>>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::vector<char> bytes = common::parseArray<char>(line);
		std::vector<int> row;
		std::transform(bytes.begin(), bytes.end(), std::back_inserter(row), [](const char byte) {
			return byte - '0';
		});
		data.push_back(row);
	}
	return data;
}

struct Node {
	int row;
	int col;
	int distance;
};

bool operator<(const Node& n1, const Node& n2) {
	if (n1.distance == n2.distance) {
		return n1.col != n2.col ? n1.col < n2.col : n1.row < n2.row;
	}
	return n1.distance < n2.distance;
}

int shortestPath(const DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();
	const int maxDistance = cols * rows * 10;

	auto distances = std::vector<std::vector<int>>(rows, std::vector<int>(cols, maxDistance));
	std::set<Node> nodes;
	nodes.insert(Node{ 0, 0, 0 });
	distances[0][0] = 0;

	while (!nodes.empty()) {
		auto node = *nodes.begin();
		nodes.erase(nodes.begin());
		auto allNeighbors = common::findNeighbors(data, node.row, node.col, 1, common::SearchPolicy::ADJACENT);
		for (const auto& neighbor : allNeighbors) {
			int nR = neighbor.row;
			int nC = neighbor.col;
			int neighborValue = neighbor.value;

			int nodeDistance = distances[node.row][node.col];
			int newDistance = nodeDistance + neighborValue;
			if (distances[nR][nC] > newDistance) {
				auto found = nodes.find(Node{ nR, nC, distances[nR][nC] });
				if (found != nodes.end()) {
					nodes.erase(found);
				}
				nodes.insert(Node{ nR, nC, newDistance });
				distances[nR][nC] = newDistance;
			}
		}
	}

	return distances[rows - 1][cols - 1];
}

int partOne(const DataType& data) {
	int risk = shortestPath(data);
	return risk;
}

int partTwo(const DataType& data) {
	const int rows = data.size();
	const int cols = data[0].size();
	DataType newData(rows * 5, std::vector<int>(cols*5));

	for (int rowInc = 0; rowInc < 5; ++rowInc) {
		for (int colInc = 0; colInc < 5; ++colInc) {
			for (int row = 0; row < rows; ++row) {
				for (int col = 0; col < cols; ++col) {
					int rowIdx = rows * rowInc + row;
					int colIdx = cols * colInc + col;
					int newVal = (data[row][col] + colInc + rowInc) % 9;
					newData[rowIdx][colIdx] = newVal == 0 ? 9 : newVal;
				}
			}
		}
	}
	
	int risk = shortestPath(newData);
	return risk;
}

int main() {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}