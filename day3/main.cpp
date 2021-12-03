#include "../common/fileReader.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using DataType = std::vector<std::string>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		data.push_back(line);
	}
	return data;
}

int mostCommonBitAt(const DataType& data, int index) {
	int ones = 0;
	int zeroes = 0;
	for (std::string line : data) {
		if (line[index] == '0') {
			++zeroes;
		} else {
			++ones;
		}
	}
	return ones >= zeroes;
}

void partOne(const DataType& data) {
	int gamma = 0;
	int epsilon = 0;
	
	int bits = data[0].size();
	for (int bit = 0; bit < bits; ++bit) {
		int mostCommonBit = mostCommonBitAt(data, bit);
		if (mostCommonBit == 1) {
			gamma |= (1 << bit);
		} else {
			epsilon |= (1 << bit);
		}
	}
	std::cout << "Part one: " << gamma << " " << epsilon << " " << gamma*epsilon << std::endl;
}

int binToInt(std::string binary) {
	int value = 0;
	for (char bit : binary) {
		value <<= 1;
		if (bit == '1') {
			value |= 1;
		}
	}
	return value;
}

void partTwo(const DataType& data) {
	int bits = data[0].size();
	DataType oxygenData(data);
	DataType coData(data);
	for (int bit = 0; bit < bits; ++bit) {
		if (oxygenData.size() > 1) {
			int mostCommonBit = mostCommonBitAt(oxygenData, bit);
			DataType newOxygenData;
			std::copy_if(oxygenData.begin(), oxygenData.end(), std::back_inserter(newOxygenData), [mostCommonBit, bit](const auto& line) {return (line[bit]-'0') == mostCommonBit; });
			oxygenData = newOxygenData;
		}
		if (coData.size() > 1) {
			int mostCommonBit = mostCommonBitAt(coData, bit);
			DataType newCoData;
			std::copy_if(coData.begin(), coData.end(), std::back_inserter(newCoData), [mostCommonBit, bit](const auto& line) {return (line[bit]-'0') != mostCommonBit; });
			coData = newCoData;
		}
		if (coData.size() == 1 && oxygenData.size() == 1) {
			break;
		}
	}
	
	int oxygenRating = binToInt(oxygenData[0]);
	int co2Rating = binToInt(coData[0]);
	std::cout << "Part two: " << oxygenRating  << " " << co2Rating << " " << oxygenRating * co2Rating << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}