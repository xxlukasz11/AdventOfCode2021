#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>

using Number = common::BinaryNode<int64_t>;

std::shared_ptr<Number> parseNumber(const std::string& line) {
	if (line.size() == 1) {
		return std::make_shared<Number>(line[0] - '0');
	}

	int comaPos = 0;
	for (int i = 0, braceIndex = 0; i < line.size(); ++i) {
		char c = line[i];
		if (c == '[') {
			++braceIndex;
		}
		else if (c == ']') {
			--braceIndex;
		}
		if (c == ',' && braceIndex == 1) {
			comaPos = i;
		}
	}

	auto beginLeft = std::next(line.begin());
	auto endRight = std::prev(line.end());
	auto endLeft = std::next(line.begin(), comaPos);
	auto beginRight = std::next(line.begin(), comaPos + 1);
	std::shared_ptr<Number> left = parseNumber(std::string(beginLeft, endLeft));
	std::shared_ptr<Number> right = parseNumber(std::string(beginRight, endRight));
	return std::make_shared<Number>(left, right);
}

using DataType = std::vector<std::shared_ptr<Number>>;
DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		std::shared_ptr<Number> number = parseNumber(line);
		data.push_back(number);
	}
	return data;
}

bool shouldExplodeThisNumber(const Number& number, int depth) {
	return depth == 4 && !number.isLeaf() && number.left().isLeaf() && number.right().isLeaf();
}

bool tryExplodeChilds(Number& number, int depth) {
	if (number.isLeaf()) {
		return false;
	}

	auto& left = number.left();
	auto& right = number.right();
	if (shouldExplodeThisNumber(left, depth + 1)) {
		auto childRightValue = left.right().value();
		auto childLeftValue = left.left().value();
		right.transformLeftmostLeaf([childRightValue](auto& leaf) {
			leaf.value() += childRightValue;
		});
		left.removeChildren();
		left.setValue(0);
		number.transformRightmostLeafLeftBranch([childLeftValue](auto& leaf) {
			leaf.value() += childLeftValue;
		});
		return true;
	}
	bool exploded = tryExplodeChilds(number.left(), depth + 1);
	if (exploded) {
		return true;
	}

	if (shouldExplodeThisNumber(right, depth + 1)) {
		auto childRightValue = right.right().value();
		auto childLeftValue = right.left().value();
		left.transformRightmostLeaf([childLeftValue](auto& leaf) {
			leaf.value() += childLeftValue;
		});
		right.removeChildren();
		right.setValue(0);
		number.transformLeftmostLeafRightBranch([childRightValue](auto& leaf) {
			leaf.value() += childRightValue;
		});
		return true;
	}

	return tryExplodeChilds(number.right(), depth + 1);
}

bool trySplitChilds(Number& number) {
	if (number.isLeaf()) {
		return false;
	}

	auto& left = number.left();
	auto& right = number.right();
	if (left.isLeaf() && left.value() > 9) {
		auto prevValue = left.value();
		auto childLeft = std::make_shared<Number>(prevValue / 2);
		auto childRight = std::make_shared<Number>(prevValue - prevValue/2);
		auto newLeft = std::make_shared<Number>(childLeft, childRight);
		number.setLeft(newLeft);
		return true;
	}

	bool splitted = trySplitChilds(left);
	if (splitted) {
		return true;
	}

	if (right.isLeaf() && right.value() > 9) {
		auto prevValue = right.value();
		auto childLeft = std::make_shared<Number>(prevValue / 2);
		auto childRight = std::make_shared<Number>(prevValue - prevValue / 2);
		auto newRight = std::make_shared<Number>(childLeft, childRight);
		number.setRight(newRight);
		return true;
	}
	return trySplitChilds(right);
}

bool reduce(Number& number) {
	bool exploded = tryExplodeChilds(number, 0);
	if (!exploded) {
		return trySplitChilds(number);
	}
	return true;
}

void reduceAll(Number& number) {
	bool reduced = true;
	while (reduced) {
		reduced = reduce(number);
	}
}

int64_t magnitude(const int64_t& left, const int64_t& right) {
	return 3 * left + 2 * right;
}

int64_t addAndCalculateMagnitude(const Number& left, const Number& right) {
	auto cl = left.clone();
	auto cr = right.clone();
	auto number = std::make_shared<Number>(cl, cr);
	reduceAll(*number);
	return number->reduceLeafs<int64_t>(magnitude);
}

int64_t partOne(const DataType& data) {
	auto left = data[0]->clone();
	for (int i = 1; i < data.size(); ++i) {
		auto right = data[i]->clone();
		auto newLeft = std::make_shared<Number>(left, right);
		reduceAll(*newLeft);
		left = newLeft;
	}
	return left->reduceLeafs<int64_t>(magnitude);
}

int64_t partTwo(const DataType& data) {
	int64_t largestMagnitude = 0;
	for (const auto& left : data) {
		for (const auto& right : data) {
			auto magnitude = addAndCalculateMagnitude(*left, *right);
			if (magnitude > largestMagnitude) {
				largestMagnitude = magnitude;
			}
		}
	}
	return largestMagnitude;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}