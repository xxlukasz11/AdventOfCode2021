#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <map>
#include <stack>
#include <cstdint>

struct Octopus {
	int energy;
	bool flashed = false;
};

using DataType = std::vector<std::vector<Octopus>>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::istringstream ss(line);
		std::vector<Octopus> row;
		for (char c; ss >> c;) {
			row.push_back(Octopus{c - '0'});
		}
		data.push_back(row);
	}
	return data;
}

void flash(DataType& data, int r, int c, int& flashCounter);

void neighborFlashed(DataType& data, int r, int c, int& flashCounter) {
	auto& [energy, flashed] = data[r][c];
	if (flashed) {
		return;
	}
	energy += 1;
	if (energy > 9) {
		flash(data, r, c, flashCounter);
	}
}

void flash(DataType& data, int r, int c, int& flashCounter) {
	auto& [energy, flashed] = data[r][c];
	if (flashed) {
		return;
	}
	flashed = true;
	energy = 0;
	++flashCounter;

	const int rows = data.size();
	const int cols = data[0].size();
	if (r - 1 >= 0) {
		if (c - 1 >= 0) {
			neighborFlashed(data, r - 1, c - 1, flashCounter);
		}
		if (c + 1 < cols) {
			neighborFlashed(data, r - 1, c + 1, flashCounter);
		}
		neighborFlashed(data, r - 1, c, flashCounter);
	}

	if (r + 1 < rows) {
		if (c - 1 >= 0) {
			neighborFlashed(data, r + 1, c - 1, flashCounter);
		}
		if (c + 1 < cols) {
			neighborFlashed(data, r + 1, c + 1, flashCounter);
		}
		neighborFlashed(data, r + 1, c, flashCounter);
	}

	if (c - 1 >= 0) {
		neighborFlashed(data, r, c - 1, flashCounter);
	}
	if (c + 1 < cols) {
		neighborFlashed(data, r, c + 1, flashCounter);
	}
}

void nextStep(DataType& data, int& flashCounter) {
	const int rows = data.size();
	const int cols = data[0].size();
	for (auto& row : data) {
		for (auto& [energy, flashed] : row) {
			energy += 1;
			flashed = false;
		}
	}

	bool someFlashHappened = true;
	while (someFlashHappened) {
		someFlashHappened = false;
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				auto& [energy, flashed] = data[r][c];
				if (energy > 9 && !flashed) {
					someFlashHappened = true;
					flash(data, r, c, flashCounter);
				}
			}
		}
	}
}

void partOne(DataType data) {
	int flashCounter = 0;
	for (int step = 0; step < 100; ++step) {
		nextStep(data, flashCounter);
	}
	std::cout << "Part one: " << flashCounter << std::endl;
}

void partTwo(DataType data) {
	const int totalOctpuses = data.size() * data[0].size();
	int flashCounter = 0;
	bool allNotFlashed = true;
	int step = 0;
	while (allNotFlashed) {
		++step;
		nextStep(data, flashCounter);

		int flashedInThisStep = 0;
		for (const auto& row : data) {
			for (const auto [_, flashed] : row) {
				flashedInThisStep += flashed;
			}
		}
		if (totalOctpuses == flashedInThisStep) {
			allNotFlashed = false;
		}
	}
	std::cout << "Part two: " << step << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}