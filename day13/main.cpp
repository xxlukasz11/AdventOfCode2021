#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <set>

enum class Fold {
	HORIZONTAL,
	VERTICAL
};

struct Data {
	std::set<std::pair<int, int>> dots;
	std::vector<std::pair<Fold, int>> folds;
};

using DataType = Data;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line) && !line.empty();) {
		auto coords = common::parseArray<int>(line, ',');
		data.dots.emplace(coords[0], coords[1]);
	}
	for (std::string line; reader.nextLine(line);) {
		auto eq = line.find('=');
		char axis = line.at(eq - 1);
		int val = std::stoi(line.substr(eq+1));
		if (axis == 'x') {
			data.folds.emplace_back(Fold::VERTICAL, val);
		} else {
			data.folds.emplace_back(Fold::HORIZONTAL, val);
		}
	}
	return data;
}

std::set<std::pair<int, int>> verticalFold(const std::set<std::pair<int, int>>& dots, int coord) {
	std::set<std::pair<int, int>> newDots;
	for (auto [x, y] : dots) {
		if (x < coord) {
			newDots.emplace(x, y);
		} else if (x > coord) {
			auto newX = coord - (x - coord);
			newDots.emplace(newX, y);
		}
	}
	return newDots;
}

std::set<std::pair<int, int>> horizontalFold(const std::set<std::pair<int, int>>& dots, int coord) {
	std::set<std::pair<int, int>> newDots;
	for (auto [x, y] : dots) {
		if (y < coord) {
			newDots.emplace(x, y);
		} else if (y > coord) {
			auto newY = coord - (y - coord);
			newDots.emplace(x, newY);
		}
	}
	return newDots;
}

std::set<std::pair<int, int>> doFold(const std::set<std::pair<int, int>>& dots, std::pair<Fold, int> foldData) {
	auto [axis, coord] = foldData;
	if (axis == Fold::VERTICAL) {
		return verticalFold(dots, coord);
	} else {
		return horizontalFold(dots, coord);
	}
}

void partOne(const DataType& data) {
	auto finalDots = doFold(data.dots, data.folds[0]);
	std::cout << "Part one: " << finalDots.size() << std::endl;
}

void print(std::set<std::pair<int, int>> points) {
	std::vector<std::pair<int, int>> verticallySortedPoints(points.begin(), points.end());
	std::sort(verticallySortedPoints.begin(), verticallySortedPoints.end(), [](const auto l, const auto r) {
		return l.second != r.second ? (l.second < r.second) : (l.first < r.first);
	});
	int lineIndex = 0;
	int charIndex = 0;
	for (auto [x, y] : verticallySortedPoints) {
		if (lineIndex != y) {
			for (int i = 0; i < y - lineIndex; ++i) {
				std::cout << std::endl;
			}
			lineIndex = y;
		}
		if (charIndex != x) {
			for (int i = 1; i < x - charIndex; ++i) {
				std::cout << " ";
			}
			charIndex = x;
		}
		std::cout << '#';
	}
	std::cout << std::endl;
}

void partTwo(const DataType& data) {
	auto finalDots = data.dots;
	for (auto fold : data.folds) {
		finalDots = doFold(finalDots, fold);
	}

	std::cout << "Part two: " << std::endl;
	print(finalDots);
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}