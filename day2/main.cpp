#include "../common/fileReader.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


enum class Dir {
	HORIZONTAL,
	VERTICAL
};

struct Command {
	Dir dir;
	int value;
};

using DataType = std::vector<Command>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::istringstream ss(line);
		std::string dir;
		ss >> dir;
		int value;
		ss >> value;
		if (dir == "forward") {
			data.push_back({ Dir::HORIZONTAL, value });
		} else if (dir == "down") {
			data.push_back({ Dir::VERTICAL, value });
		} else {
			data.push_back({ Dir::VERTICAL, -value });
		}
	}
	return data;
}

void partOne(const DataType& data) {
	int horizontal = 0;
	int depth = 0;
	for (auto [dir, value] : data) {
		if (dir == Dir::HORIZONTAL) {
			horizontal += value;
		} else {
			depth += value;
		}
	}
	std::cout << "Part one: " << horizontal * depth << std::endl;
}

void partTwo(const DataType& data) {
	int horizontal = 0;
	int depth = 0;
	int aim = 0;
	for (auto [dir, value] : data) {
		if (dir == Dir::HORIZONTAL) {
			horizontal += value;
			depth += aim * value;
		} else {
			aim += value;
		}
	}
	std::cout << "Part two: " << horizontal * depth << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}