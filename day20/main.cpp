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
#include <deque>

using ImageType = std::deque<std::deque<bool>>;

struct Data {
	std::vector<bool> rule;
	ImageType image;
};

using DataType = Data;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	std::string line = reader.nextLine();
	auto rule = common::parseArray<char>(line);
	for (const char c : rule) {
		data.rule.push_back(c == '#');
	}
	for (std::string line; reader.nextLine(line);) {
		if (line.empty()) {
			continue;
		}
		auto rowChars = common::parseArray<char>(line);
		std::deque<bool> row;
		std::transform(rowChars.begin(), rowChars.end(), std::back_inserter(row), [](const char c) {
			return c == '#';
		});
		data.image.push_back(row);
	}
	return data;
}

void expand(ImageType& image, bool value = false) {
	const int expectedSize = image[0].size() + 2;
	for (auto& row : image) {
		row.push_back(value);
		row.push_front(value);
	}
	image.push_back(std::deque<bool>(expectedSize, value));
	image.push_front(std::deque<bool>(expectedSize, value));
}

void appendNeighbor(int& rule, bool value, int neighborIdx) {
	rule |= (value << neighborIdx);
}

void setFrame(ImageType& image, bool value) {
	const int rows = image.size();
	const int cols = image[0].size();
	image[0] = std::deque<bool>(cols, value);
	image[rows-1] = std::deque<bool>(cols, value);
	for (int i = 1; i < rows - 1; ++i) {
		image[i][0] = value;
		image[i][cols - 1] = value;
	}
}

void nextStep(ImageType& image, const std::vector<bool>& rule, bool isOddStep) {
	if (isOddStep) {
		expand(image, true);
	}
	else {
		expand(image);
	}

	static ImageType snapshot;
	snapshot = image;

	const int rows = image.size();
	const int cols = image[0].size();
	constexpr int offset = 1;
	for (int rowIdx = offset; rowIdx < rows - offset; ++rowIdx) {
		for (int colIdx = offset; colIdx < cols - offset; ++colIdx) {
			int ruleIdx = 0;
			appendNeighbor(ruleIdx, snapshot[rowIdx - 1][colIdx - 1], 8);
			appendNeighbor(ruleIdx, snapshot[rowIdx - 1][colIdx], 7);
			appendNeighbor(ruleIdx, snapshot[rowIdx - 1][colIdx + 1], 6);
			appendNeighbor(ruleIdx, snapshot[rowIdx][colIdx - 1], 5);
			appendNeighbor(ruleIdx, snapshot[rowIdx][colIdx], 4);
			appendNeighbor(ruleIdx, snapshot[rowIdx][colIdx + 1], 3);
			appendNeighbor(ruleIdx, snapshot[rowIdx + 1][colIdx - 1], 2);
			appendNeighbor(ruleIdx, snapshot[rowIdx + 1][colIdx], 1);
			appendNeighbor(ruleIdx, snapshot[rowIdx + 1][colIdx + 1], 0);
			const auto newValue = rule[ruleIdx];
			image[rowIdx][colIdx] = newValue;
		}
	}
	if (isOddStep) {
		setFrame(image, false);
	}
	else {
		setFrame(image, true);
	}
}

int countPixels(const ImageType& image) {
	int pixelCount = 0;
	for (const auto& row : image) {
		for (const bool pixel : row) {
			pixelCount += pixel;
		}
	}
	return pixelCount;
}

int partOne(const DataType& data) {
	ImageType image = data.image;
	const auto& rule = data.rule;

	expand(image);
	for (int step = 0; step < 2; ++step) {
		nextStep(image, rule, step % 2 == 1);
	}

	return countPixels(image);
}

int partTwo(const DataType& data) {
	ImageType image = data.image;
	const auto& rule = data.rule;

	expand(image);
	for (int step = 0; step < 50; ++step) {
		nextStep(image, rule, step % 2 == 1);
	}

	return countPixels(image);
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}