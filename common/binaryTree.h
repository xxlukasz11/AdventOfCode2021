#pragma once

#include <memory>
#include <functional>

namespace common {

template<typename ValueT>
class BinaryNode {

	ValueT value_;
	std::shared_ptr<BinaryNode> left_;
	std::shared_ptr<BinaryNode> right_;
	BinaryNode* parent_{ nullptr };
	bool isLeaf_{ true };

public:
	explicit BinaryNode(const ValueT& value) : value_(value), isLeaf_(true) {}

	explicit BinaryNode(const std::shared_ptr<BinaryNode>& left, const std::shared_ptr<BinaryNode>& right) :
			left_(left), right_(right), isLeaf_(false) {
		left_->parent_ = this;
		right_->parent_ = this;
	}

	explicit BinaryNode(const BinaryNode& node) {
		if (node.isLeaf()) {
			value_ = node.value_;
			isLeaf_ = true;
		}
		else {
			left_ = std::make_shared<BinaryNode>(*node.left_);
			right_ = std::make_shared<BinaryNode>(*node.right_);
			left_->parent_ = this;
			right_->parent_ = this;
			isLeaf_ = false;
		}
	}

	bool isLeaf() const {
		return isLeaf_;
	}

	bool hasParent() const {
		return parent_ != nullptr;
	}

	bool hasLeft() const {
		return left_;
	}

	bool hasRight() const {
		return right_;
	}

	void setValue(int64_t value) {
		value_ = value;
	}

	void setLeft(const std::shared_ptr<BinaryNode>& left) {
		left_ = left;
		left_->parent_ = this;
		isLeaf_ = false;
	}

	void setRight(const std::shared_ptr<BinaryNode>& right) {
		right_ = right;
		right_->parent_ = this;
		isLeaf_ = false;
	}

	BinaryNode& parent() {
		return *parent_;
	}

	const BinaryNode& parent() const {
		return *parent_;
	}

	BinaryNode& left() {
		return *left_;
	}

	const BinaryNode& left() const {
		return *left_;
	}

	BinaryNode& right() {
		return *right_;
	}

	const BinaryNode& right() const {
		return *right_;
	}

	ValueT value() const {
		return value_;
	}

	void removeChildren() {
		left_.reset();
		right_.reset();
		isLeaf_ = true;
	}

	std::shared_ptr<BinaryNode> clone() const {
		auto node = std::make_shared<BinaryNode>(value_);
		if (hasLeft()) {
			node->setLeft(std::make_shared<BinaryNode>(*left_));
		}
		if (hasRight()) {
			node->setRight(std::make_shared<BinaryNode>(*right_));
		}
		return node;
	}

	void transformLeftmostLeaf(std::function<void(BinaryNode&)> func) {
		if (isLeaf()) {
			func(*this);
		}
		else if (hasLeft()) {
			left_->transformLeftmostLeaf(func);
		}
		else {
			right_->transformLeftmostLeaf(func);
		}
	}

	void transformRightmostLeaf(std::function<void(BinaryNode&)> func) {
		if (isLeaf()) {
			func(*this);
		}
		else if (hasRight()) {
			right_->transformLeftmostLeaf(func);
		}
		else {
			left_->transformLeftmostLeaf(func);
		}
	}

	void transformLeftmostLeafRightBranch(std::function<void(BinaryNode&)> func) {
		if (!hasParent()) {
			return;
		}
		if (parent_->hasRight() && parent_->right_.get() == this) {
			parent_->transformLeftmostLeafRightBranch(func);
		}
		else {
			parent_->right()->transformLeftmostLeaf(func);
		}
	}

	void transformRightmostLeafLeftBranch(std::function<void(BinaryNode&)> func) {
		if (!hasParent()) {
			return;
		}
		if (parent_->hasLeft() && parent_->left_.get() == this) {
			parent_->transformRightmostLeafLeftBranch(func);
		}
		else {
			parent_->left()->transformRightmostLeaf(func);
		}
	}

	std::string toString(char braceLeft, char braceRight) const {
		if (isLeaf()) {
			return std::to_string(value_.value());
		}
		if (hasRight() && hasLeft()) {
			return braceLeft + left_->toString(braceLeft, braceRight) + ',' + right_->toString(braceLeft, braceRight) + braceRight;
		}
		if (hasRight()) {
			return braceLeft + right_->toString(braceLeft, braceRight) + braceRight;
		}
		if (hasLeft()) {
			return braceLeft + left_->toString(braceLeft, braceRight) + braceRight;
		}
	}

};

} // namespace