#include "../common/pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>

struct Cave {
	std::string name;
	bool isLowerCase;
};
using DataType = std::multimap<std::string, Cave>;

static const std::string START = "start";
static const std::string END = "end";

bool isLowerCase(std::string cave) {
	for (char c : cave) {
		if (c < 'a') {
			return false;
		}
	}
	return true;
}

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		auto delim = line.find('-');
		auto first = line.substr(0, delim);
		auto second = line.substr(delim + 1);
		bool lcFirst = isLowerCase(first);
		bool lcSecond = isLowerCase(second);
		data.insert({ first, Cave{second, lcSecond} });
		data.insert({ second , Cave{first, lcFirst} });
	}
	return data;
}

std::vector<std::vector<std::string>> moveToTheNextState(
		const std::vector<std::string>& path,
		const std::set<std::string>& visitedLowerCase,
		const DataType& data) {
	if (path.back() == END) {
		return { path };
	}
	std::vector<std::vector<std::string>> allPaths;
	auto possbileCaves = data.equal_range(path.back());
	for (auto it = possbileCaves.first; it != possbileCaves.second; ++it) {
		Cave cave = it->second;
		if (cave.name == START) {
			continue;
		}
		if (cave.isLowerCase && visitedLowerCase.find(cave.name) != visitedLowerCase.end()) {
			continue;
		}
		std::set<std::string> visitedLC(visitedLowerCase.begin(), visitedLowerCase.end());
		if (cave.isLowerCase) {
			visitedLC.insert(cave.name);
		}
		std::vector<std::string> currentPath(path.begin(), path.end());
		currentPath.push_back(cave.name);
		std::vector<std::vector<std::string>> paths = moveToTheNextState(currentPath, visitedLC, data);
		std::copy(paths.begin(), paths.end(), std::back_inserter(allPaths));
	}
	return allPaths;
}

void partOne(const DataType& data) {
	std::vector<std::string> startingPath{ START };
	std::set<std::string> visitedLowerCase;
	std::vector<std::vector<std::string>> paths = moveToTheNextState(startingPath, visitedLowerCase, data);
	std::cout << "Part one: " << paths.size() << std::endl;
}

std::vector<std::vector<std::string>> moveToTheNextStatePartTwo(
	const std::vector<std::string>& path,
	const std::set<std::string>& visitedLowerCase,
	const DataType& data,
	bool smallCaveVisitedTwice) {
	if (path.back() == END) {
		return { path };
	}
	std::vector<std::vector<std::string>> allPaths;
	auto possbileCaves = data.equal_range(path.back());
	for (auto it = possbileCaves.first; it != possbileCaves.second; ++it) {
		Cave cave = it->second;
		if (cave.name == START) {
			continue;
		}
		bool smallCaveVisitedTwiceCurrentPath = smallCaveVisitedTwice;
		if (cave.isLowerCase && visitedLowerCase.find(cave.name) != visitedLowerCase.end()) {
			if (smallCaveVisitedTwice) {
				continue;
			} else {
				smallCaveVisitedTwiceCurrentPath = true;
			}
		}
		std::set<std::string> visitedLowerCaseCurrentPath(visitedLowerCase.begin(), visitedLowerCase.end());
		if (cave.isLowerCase) {
			visitedLowerCaseCurrentPath.insert(cave.name);
		}
		std::vector<std::string> currentPath(path.begin(), path.end());
		currentPath.push_back(cave.name);
		std::vector<std::vector<std::string>> paths = moveToTheNextStatePartTwo(currentPath,
			visitedLowerCaseCurrentPath, data, smallCaveVisitedTwiceCurrentPath);
		std::copy(paths.begin(), paths.end(), std::back_inserter(allPaths));
	}
	return allPaths;
}

void partTwo(const DataType& data) {
	std::vector<std::string> startingPath{ START };
	std::set<std::string> visitedLowerCase;
	std::vector<std::vector<std::string>> paths = moveToTheNextStatePartTwo(startingPath, visitedLowerCase, data, false);
	std::cout << "Part two: " << paths.size() << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}