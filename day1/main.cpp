#include "../common/fileReader.h"
#include <iostream>
#include <vector>
#include <string>

using DataType = std::vector<int>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		auto value = std::stoi(line);
		data.push_back(value);
	}
	return data;
}

int countIncreased(const DataType& data) {
	int increased = 0;
	auto current = data.begin()++;
	auto end = data.end();
	while (current != end) {
		if (*std::prev(current) < *current) {
			++increased;
		}
		++current;
	}
	return increased;
}

void partOne(const DataType& data) {
	int increased = countIncreased(data);
	std::cout << "Part one: " << increased << std::endl;
}

void partTwo(const DataType& data) {
	DataType windowData;
	windowData.reserve(data.size());
	int dataSize = data.size();
	for (int i = 0; i < dataSize; ++i) {
		windowData.push_back(data[i]);
		if (i < dataSize - 1 && i - 1 >= 0) {
			windowData[i] += data[i - 1];
		}
		if (i < dataSize - 2 && i - 2 >= 0) {
			windowData[i] += data[i - 2];
		}
	}
	int increased = countIncreased(windowData);
	std::cout << "Part two: " << increased << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}