#include "../common/pch.h"
#include <iostream>
#include <string>
#include <sstream>

struct Data {
	int minX;
	int maxX;
	int minY;
	int maxY;
};

using DataType = Data;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	std::string line = reader.nextLine();
	auto eq = line.find('=');
	std::istringstream ss(line.substr(eq + 1));
	ss >> data.minX;
	ss.ignore(2);
	ss >> data.maxX;
	ss.ignore(4);
	ss >> data.minY;
	ss.ignore(2);
	ss >> data.maxY;
	return data;
}

int findMinXSpeed(int xMin, int xMax) {
	int initialSpeed = 0;
	while (true) {
		++initialSpeed;
		int currentSpeed = initialSpeed;
		int x = 0;
		while (currentSpeed > 0) {
			x += currentSpeed;
			--currentSpeed;
		}
		if (x <= xMax && x >= xMin) {
			return initialSpeed;
		}
	}
	throw "So bad";
}

bool checkIfHitTheTarget(const DataType& data, const int vx, const int vy) {
	int currentXSpeed = vx;
	int currentYSpeed = vy;
	int y = 0;
	int x = 0;
	while (true) {
		y += currentYSpeed;
		x += currentXSpeed;
		--currentYSpeed;
		if (currentXSpeed > 0) {
			--currentXSpeed;
		}

		if (x > data.maxX || y < data.minY) {
			return false;
		}
		else if (x >= data.minX && y <= data.maxY) {
			return true;
		}
	}
}

int partOne(const DataType& data) {
	int speedY = -data.minY - 1;
	int y = 0;
	int maxY = 0;
	while (y > data.maxY) {
		y += speedY;
		--speedY;
		if (maxY < y) {
			maxY = y;
		}
	}
	return maxY;
}

int partTwo(const DataType& data) {
	const int vyMax = -data.minY - 1;
	const int vyMin = data.minY;
	const int vxMin = findMinXSpeed(data.minX, data.maxX);
	const int vxMax = data.maxX;

	int noOfTrajectories = 0;
	for (int vx = vxMin; vx <= vxMax; ++vx) {
		for (int vy = vyMin; vy <= vyMax; ++vy) {
			noOfTrajectories += checkIfHitTheTarget(data, vx, vy);
		}
	}
	return noOfTrajectories;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}