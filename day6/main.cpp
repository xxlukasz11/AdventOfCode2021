#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <numeric>

using DataType = std::vector<int>;

DataType read() {
	common::FileReader reader("input.txt");
	std::string line = reader.nextLine();
	DataType data = common::parseArray<int>(line, ',');
	return data;
}

void partOne(DataType data) {
	for (int i = 0; i < 80; ++i) {
		DataType newOnes;
		for (int& val : data) {
			if (val == 0) {
				newOnes.push_back(8);
				val = 6;
			} else {
				--val;
			}
		}
		std::copy(newOnes.begin(), newOnes.end(), std::back_inserter(data));
	}
	std::cout << "Part one: " << data.size() << std::endl;
}

void partTwo(DataType data) {
	std::vector<uint64_t> initialFishTimer(9, 0);
	for (int val : data) {
		initialFishTimer[val] += 1;
	}
	std::list<uint64_t> fishTimer(initialFishTimer.begin(), initialFishTimer.end());
	for (int i = 0; i < 256; ++i) {
		auto newFishCount = fishTimer.front();
		std::rotate(fishTimer.begin(), ++(fishTimer.begin()), fishTimer.end());
		auto front = fishTimer.begin();
		std::advance(front, 6);
		*front += newFishCount;
	}
	auto sum = std::accumulate(fishTimer.begin(), fishTimer.end(), uint64_t{});
	std::cout << "Part two: " << sum << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}