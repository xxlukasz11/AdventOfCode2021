#include "../common/fileReader.h"
#include "../common/utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

struct Data {
	std::vector<std::string> combinations;
	std::vector<std::string> digits;
};

using DataType = std::vector<Data>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::vector<std::string> combinations = common::parseArray<std::string>(line);
		auto delimiter = line.find('|');
		std::string restOfTheLine = line.substr(delimiter + 1);
		std::vector<std::string> digits = common::parseArray<std::string>(restOfTheLine);
		data.push_back(Data{combinations, digits});
	}
	return data;
}

void partOne(const DataType& data) {
	int count = 0;
	for (const auto& [combinations, digits] : data) {
		for (const auto digit : digits) {
			const auto digitSize = digit.size();
			if (digitSize == 2 || digitSize == 4 || digitSize == 3 || digitSize == 7) {
				++count;
			}
		}
	}
	std::cout << "Part one: " << count << std::endl;
}

int decodeDigit(const std::string digit, char topRightChar, char bottomLeft) {
	if (digit.size() == 2) {
		return 1;
	}
	if (digit.size() == 3) {
		return 7;
	}
	if (digit.size() == 4) {
		return 4;
	}
	if (digit.size() == 7) {
		return 8;
	}
	if (digit.size() == 6) {
		if (digit.find(topRightChar) != std::string::npos) {
			if (digit.find(bottomLeft) != std::string::npos) {
				return 0;
			}
			else {
				return 9;
			}
		}
		else {
			return 6;
		}
	}
	if (digit.find(topRightChar) != std::string::npos) {
		if (digit.find(bottomLeft) != std::string::npos) {
			return 2;
		}
		else {
			return 3;
		}
	}
	else {
		return 5;
	}
}

std::string uniqueChars(std::string first, std::string second) {
	std::map<char, int> charCount;
	for (char c : first) {
		charCount[c] += 1;
	}
	for (char c : second) {
		charCount[c] += 1;
	}
	std::vector<char> unique;
	for (auto [key, value] : charCount) {
		if (value == 1) {
			unique.push_back(key);
		}
	}
	return std::string(unique.begin(), unique.end());
}

std::string commonChars(std::string first, std::string second) {
	std::map<char, int> charCount;
	for (char c : first) {
		charCount[c] += 1;
	}
	for (char c : second) {
		charCount[c] += 1;
	}
	std::vector<char> unique;
	for (auto [key, value] : charCount) {
		if (value == 2) {
			unique.push_back(key);
		}
	}
	return std::string(unique.begin(), unique.end());
}

std::pair<std::string, std::string> findSixAndNine(const std::vector<std::string>& combinations, std::string four, std::string seven) {
	std::string six;
	std::string nine;
	for (std::string combination : combinations) {
		if (combination.size() != 6) {
			continue;
		}
		if (commonChars(combination, seven).size() == 2) {
			six = combination;
		}
		else if (commonChars(combination, four).size() == 4) {
			nine = combination;
		}
	}
	return { six, nine };
}

std::string findCombinationOfSize(int size, const std::vector<std::string>& combinations) {
	return *std::find_if(combinations.begin(), combinations.end(), [size](const auto combination) {
		return combination.size() == size;
	});
}

void partTwo(const DataType& data) {
	int sum = 0;
	for (const auto& [combinations, digits] : data) {
		auto four = findCombinationOfSize(4, combinations);
		auto seven = findCombinationOfSize(3, combinations);
		auto eight = findCombinationOfSize(7, combinations);
		auto [six, nine] = findSixAndNine(combinations, four, seven);
		char topRight = uniqueChars(six, eight)[0];
		char bottomLeft = uniqueChars(nine, eight)[0];

		int d1 = decodeDigit(digits[0], topRight, bottomLeft);
		int d2 = decodeDigit(digits[1], topRight, bottomLeft);
		int d3 = decodeDigit(digits[2], topRight, bottomLeft);
		int d4 = decodeDigit(digits[3], topRight, bottomLeft);
		sum += d1 * 1000 + d2 * 100 + d3 * 10 + d4;
	}
	std::cout << "Part two: " << sum << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}