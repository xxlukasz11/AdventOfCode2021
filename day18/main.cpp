#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <vector>
#include <map>
#include <set>

class Number {
	std::optional<int64_t> value_;
	std::shared_ptr<Number> left_;
	std::shared_ptr<Number> right_;
	Number* parent_{ nullptr };

public:
	Number(int64_t value) : value_(value) {}

	Number(const std::shared_ptr<Number>& left, const std::shared_ptr<Number>& right) :
		left_(left), right_(right) {
		left_->parent_ = this;
		right_->parent_ = this;
	}

	Number(const Number& number) {
		if (number.isValue()) {
			value_ = number.value_.value();
		}
		else {
			left_ = std::make_shared<Number>(*number.left_);
			right_ = std::make_shared<Number>(*number.right_);
			left_->parent_ = this;
			right_->parent_ = this;
		}
	}

	bool hasParent() const {
		return parent_ != nullptr;
	}

	Number* parent() {
		return parent_;
	}

	void setValue(int64_t value) {
		value_ = value;
		left_.reset();
		right_.reset();
	}

	void setLeft(std::shared_ptr<Number> left) {
		left_ = left;
		left_->parent_ = this;
	}

	void setRight(std::shared_ptr<Number> right) {
		right_ = right;
		right_->parent_ = this;
	}

	std::shared_ptr<Number> left() const {
		return left_;
	}

	std::shared_ptr<Number> right() const {
		return right_;
	}

	int64_t value() const {
		return value_.value();
	}

	bool isValue() const {
		return value_.has_value();
	}

	int64_t magnitude() {
		if (value_.has_value()) {
			return value_.value();
		}
		return 3*left_->magnitude() + 2*right_->magnitude();
	}

	std::shared_ptr<Number> clone() const {
		if (value_.has_value()) {
			return std::make_shared<Number>(value_.value());
		}
		else {
			auto newLeft = std::make_shared<Number>(*left_);
			auto newRight = std::make_shared<Number>(*right_);
			return std::make_shared<Number>(newLeft, newRight);
		}
	}

	void addLeft(int64_t val) {
		if (value_.has_value()) {
			value_.value() += val;
		}
		else {
			left_->addLeft(val);
		}
	}

	void addRight(int64_t val) {
		if (value_.has_value()) {
			value_.value() += val;
		}
		else {
			right_->addRight(val);
		}
	}

	void addRightUpward(int64_t val) {
		if (parent_ == nullptr) {
			return;
		}
		if (parent_->right().get() == this) {
			parent_->addRightUpward(val);
		}
		else {
			parent_->right()->addLeft(val);
		}
	}

	void addLeftUpward(int64_t val) {
		if (parent_ == nullptr) {
			return;
		}
		if (parent_->left().get() == this) {
			parent_->addLeftUpward(val);
		}
		else {
			parent_->left()->addRight(val);
		}
	}

	std::string toString() const {
		if (value_.has_value()) {
			return std::to_string(value_.value());
		}
		else {
			return '[' + left_->toString() + ',' + right_->toString() + ']';
		}
	}

};

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

bool shouldExplodeThisNumber(std::shared_ptr<Number> number, int currentLevel) {
	return currentLevel == 4 && !number->isValue() && number->left()->isValue() && number->right()->isValue();
}

bool reduce(std::shared_ptr<Number> number, int currentLevel);

bool tryExplodeChilds(const std::shared_ptr<Number>& number, int currentLevel) {
	if (number->isValue()) {
		return false;
	}

	auto left = number->left();
	auto right = number->right();
	if (shouldExplodeThisNumber(left, currentLevel + 1)) {
		auto childRightValue = left->right()->value();
		auto childLeftValue = left->left()->value();
		right->addLeft(childRightValue);
		left->setValue(0);
		number->addLeftUpward(childLeftValue);
		return true;
	}
	bool exploded = tryExplodeChilds(number->left(), currentLevel + 1);
	if (exploded) {
		return true;
	}

	if (shouldExplodeThisNumber(right, currentLevel + 1)) {
		auto childRightValue = right->right()->value();
		auto childLeftValue = right->left()->value();
		left->addRight(childLeftValue);
		right->setValue(0);
		number->addRightUpward(childRightValue);
		return true;
	}

	return tryExplodeChilds(number->right(), currentLevel + 1);
}

bool trySplitChilds(const std::shared_ptr<Number>& number) {
	if (number->isValue()) {
		return false;
	}

	auto left = number->left();
	auto right = number->right();
	if (left->isValue() && left->value() > 9) {
		// split left
		auto prevValue = left->value();
		auto childLeft = std::make_shared<Number>(prevValue / 2);
		auto childRight = std::make_shared<Number>(prevValue - prevValue/2);
		auto newLeft = std::make_shared<Number>(childLeft, childRight);
		number->setLeft(newLeft);
		return true;
	}

	bool splitted = trySplitChilds(left);
	if (splitted) {
		return true;
	}

	if (right->isValue() && right->value() > 9) {
		// split right
		auto prevValue = right->value();
		auto childLeft = std::make_shared<Number>(prevValue / 2);
		auto childRight = std::make_shared<Number>(prevValue - prevValue / 2);
		auto newRight = std::make_shared<Number>(childLeft, childRight);
		number->setRight(newRight);
		return true;
	}
	return trySplitChilds(right);
}

bool reduce(std::shared_ptr<Number> number, int currentLevel) {
	bool exploded = tryExplodeChilds(number, currentLevel);
	if (!exploded) {
		return trySplitChilds(number);
	}
	return true;
}

void reduceAll(std::shared_ptr<Number> number) {
	bool reduced = true;
	while (reduced) {
		reduced = reduce(number, 0);
	}
}

int64_t addAndCalculateMagnitude(const std::shared_ptr<Number>& left, const std::shared_ptr<Number>& right) {
	auto cl = left->clone();
	auto cr = right->clone();
	auto number = std::make_shared<Number>(cl, cr);
	reduceAll(number);
	return number->magnitude();
}

int64_t partOne(const DataType& data) {
	auto left = data[0]->clone();
	for (int i = 1; i < data.size(); ++i) {
		auto right = data[i]->clone();
		auto newLeft = std::make_shared<Number>(left, right);
		reduceAll(newLeft);
		left = newLeft;
	}
	return left->magnitude();
}

int64_t partTwo(const DataType& data) {
	int64_t largestMagnitude = 0;
	for (const auto& left : data) {
		for (const auto& right : data) {
			auto magnitude = addAndCalculateMagnitude(left, right);
			if (magnitude > largestMagnitude) {
				largestMagnitude = magnitude;
			}

			auto revMagnitude = addAndCalculateMagnitude(right, left);
			if (revMagnitude > largestMagnitude) {
				largestMagnitude = revMagnitude;
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