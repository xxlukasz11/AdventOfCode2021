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
#include <list>
#include <future>

struct Data {
	std::list<char> polymerTemplate;
	std::map<std::string, char> pairInsertions;
};

using DataType = Data;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	std::string tmp = reader.nextLine();
	data.polymerTemplate = std::list<char>(tmp.begin(), tmp.end());
	for (std::string line; reader.nextLine(line);) {
		if (line.empty()) {
			continue;
		}
		std::string pair = line.substr(0, 2);
		char letter = line.at(6);
		data.pairInsertions.emplace(pair, letter);
	}
	return data;
}

std::list<char> getPolymerAtDepth(const std::map<std::string, char>& pairInsertions, std::list<char> currentpolymer, int depth) {
	for (int i = 0; i < depth; ++i) {
		auto it = currentpolymer.begin();
		while (true) {
			auto next = std::next(it);
			if (next == currentpolymer.end()) {
				break;
			}
			std::string pair;
			pair += *it;
			pair += *next;
			auto found = pairInsertions.find(pair);
			if (found == pairInsertions.end()) {
				throw "So bad";
			}
			char toInsert = found->second;

			currentpolymer.insert(next, toInsert);
			it = next;
		}
	}
	return currentpolymer;
}

void partOne(const DataType& data) {
	const auto& pairInsertions = data.pairInsertions;
	std::list<char> polymer = getPolymerAtDepth(pairInsertions, data.polymerTemplate, 10);

	std::map<char, int> elementCount;
	for (char c : polymer) {
		elementCount[c] += 1;
	}
	int maxCount = std::max_element(elementCount.begin(), elementCount.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	})->second;
	int minCount = std::min_element(elementCount.begin(), elementCount.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	})->second;

	std::cout << "Part one: " << maxCount - minCount << std::endl;
}

void partTwo(const DataType& data) {
	const auto& pairInsertions = data.pairInsertions;
	std::unordered_map<std::string, std::unordered_map<char, uint64_t>> pairToElementCount;
	for (auto pair : pairInsertions) {
		const std::string key = pair.first;
		const std::list<char> polymer = getPolymerAtDepth(pairInsertions, std::list<char>(key.begin(), key.end()), 20);
		std::unordered_map<char, uint64_t> elementCount;
		for (char c : polymer) {
			elementCount[c] += 1;
		}
		pairToElementCount.emplace(key, elementCount);
	}

	const std::list<char> polymer = getPolymerAtDepth(pairInsertions, data.polymerTemplate, 20);
	std::unordered_map<char, uint64_t> elementCount;
	const auto end = polymer.end();
	auto it = polymer.begin();
	auto next = std::next(it);
	while (next != end) {
		std::string pair;
		pair += *it;
		pair += *next;
		++it;
		++next;
		const auto pairCount = pairToElementCount.find(pair)->second;
		for (const auto el : pairCount) {
			elementCount[el.first] += el.second;
		}
	}


	// subtract overlapping elements
	auto l = std::next(polymer.begin());
	auto r = std::prev(polymer.end());
	while (l != r) {
		elementCount[*l] -= 1;
		++l;
	}
	
	uint64_t maxCount = std::max_element(elementCount.begin(), elementCount.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	})->second;
	uint64_t minCount = std::min_element(elementCount.begin(), elementCount.end(), [](const auto l, const auto r) {
		return l.second < r.second;
	})->second;

	std::cout << "Part two: " << maxCount - minCount << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}