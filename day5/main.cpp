#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

struct Point {
	int x;
	int y;

	bool operator<(Point p) const {
		if (x != p.x) {
			return x < p.x;
		} else {
			return y < p.y;
		}
	}
};

struct Line {
	Point start;
	Point end;
};

using DataType = std::vector<Line>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::istringstream ss(line);
		int x1, x2, y1, y2;
		ss >> x1;
		ss.ignore();
		ss >> y1;
		ss.ignore(4);
		ss >> x2;
		ss.ignore();
		ss >> y2;
		Line lineObject{ {x1, y1}, {x2, y2} };
		data.push_back(lineObject);
	}
	return data;
}

int sign(int value) {
	return (value > 0) - (value < 0);
}

int countOverlaps(const std::map<Point, int> map) {
	int overlapCounter = 0;
	for (const auto [_, value] : map) {
		if (value >= 2) {
			++overlapCounter;
		}
	}
	return overlapCounter;
}

void partOne(const DataType& data) {
	std::map<Point, int> map;
	for (auto [start, end] : data) {
		if (start.x != end.x && start.y != end.y) {
			continue;
		}

		int deltaX = end.x - start.x;
		int deltaY = end.y - start.y;
		int dx = sign(deltaX);
		int dy = sign(deltaY);
		int iterations = std::max(abs(deltaX), abs(deltaY)) + 1;

		for (int i = 0; i < iterations; ++i) {
			Point point{ start.x + i * dx, start.y + i * dy };
			map[point] += 1;
		}
	}

	int overlaps = countOverlaps(map);
	std::cout << "Part one: " << overlaps << std::endl;
}

void partTwo(const DataType& data) {
	std::map<Point, int> map;
	for (auto [start, end] : data) {
		int deltaX = end.x - start.x;
		int deltaY = end.y - start.y;
		int dx = sign(deltaX);
		int dy = sign(deltaY);
		int iterations = std::max(abs(deltaX), abs(deltaY)) + 1;

		for (int i = 0; i < iterations; ++i) {
			Point point{ start.x + i * dx, start.y + i * dy };
			map[point] += 1;
		}
	}

	int overlaps = countOverlaps(map);
	std::cout << "Part two: " << overlaps << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}