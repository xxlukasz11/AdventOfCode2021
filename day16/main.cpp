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

using DataType = std::vector<int>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {

	}
	return data;
}

int partOne(const DataType& data) {

	return 0;
}

int partTwo(const DataType& data) {

	return 0;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}