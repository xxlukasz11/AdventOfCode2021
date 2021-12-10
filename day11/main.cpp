#include "../common/fileReader.h"
#include "../common/utils.h"
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

using DataType = std::vector<std::string>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		data.push_back(line);
	}
	return data;
}

void partOne(const DataType& data) {
	std::map<char, int> errorMapping{
		{')', 3},
		{']', 57},
		{'}', 1197},
		{'>', 25137}
	};

	std::map<char, char> braceMapping{
		{'(', ')'},
		{'[', ']'},
		{'{', '}'},
		{'<', '>'}
	};

	int errorScore = 0;
	for (const std::string& line : data) {
		std::stack<char> stack;
		for (char c : line) {
			auto openBrace = braceMapping.find(c);
			if (openBrace != braceMapping.end()) {
				stack.push(openBrace->second);
			}
			else {
				auto expected = stack.top();
				stack.pop();
				if (expected != c) {
					errorScore += errorMapping[c];
				}

			}
		}
	}

	std::cout << "Part one: " << errorScore << std::endl;
}

void partTwo(const DataType& data) {
	std::map<char, int> pointMapping{
		{')', 1},
		{']', 2},
		{'}', 3},
		{'>', 4}
	};

	std::map<char, char> braceMapping{
		{'(', ')'},
		{'[', ']'},
		{'{', '}'},
		{'<', '>'}
	};

	std::vector<uint64_t> scores;
	for (const auto& line : data) {
		std::stack<char> stack;
		bool fail = false;
		for (auto c : line) {
			auto bracePair = braceMapping.find(c);
			if (bracePair != braceMapping.end()) {
				stack.push(bracePair->second);
			}
			else {
				auto expected = stack.top();
				stack.pop();
				if (expected != c) {
					fail = true;
					break;
				}

			}
		}

		if (fail) {
			continue;
		}

		uint64_t lineScore = 0;
		while (stack.size() > 0) {
			auto closing = stack.top();
			stack.pop();
			lineScore *= 5;
			lineScore += pointMapping[closing];
		}
		scores.push_back(lineScore);
	}
	std::sort(scores.begin(), scores.end());
	auto scoresSize = scores.size();
	auto middle = scoresSize / 2;
	std::cout << "Part two: " << scores[middle] << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}