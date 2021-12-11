#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

using DataType = std::vector<int>;

DataType read() {
	common::FileReader reader("input.txt");
	std::string line = reader.nextLine();
	DataType data = common::parseArray<int>(line, ',');
	return data;
}

void partOne(const DataType& data) {
	int min = *std::min_element(data.begin(), data.end());
	int max = *std::max_element(data.begin(), data.end());

	std::vector<std::pair<int, int>> costs;
	for (int dst = min; dst <= max; ++dst) {
		int fuel = 0;
		for (int src : data) {
			fuel += abs(src - dst);
		}
		costs.emplace_back(dst, fuel);
	}

	auto minCosts = *std::min_element(costs.begin(), costs.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	});

	std::cout << "Part one: " << minCosts.first << " " << minCosts.second << std::endl;
}

int sumFuel(int src, int dst) {
	int inc = 1;
	int sum = 0;
	int steps = abs(src - dst);
	for (int i = 0; i < steps; ++i) {
		sum += inc++;
	}
	return sum;
}

void partTwo(const DataType& data) {
	int min = *std::min_element(data.begin(), data.end());
	int max = *std::max_element(data.begin(), data.end());

	std::vector<std::pair<int, int>> costs;
	for (int dst = min; dst <= max; ++dst) {
		int fuel = 0;
		for (int src : data) {
			fuel += sumFuel(src, dst);
		}
		costs.emplace_back(dst, fuel);
	}

	auto minCosts = *std::min_element(costs.begin(), costs.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	});

	std::cout << "Part two: " << minCosts.first << " " << minCosts.second << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}