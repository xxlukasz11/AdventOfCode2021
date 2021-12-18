#pragma once

#include <memory>
#include <functional>

namespace common {

template<typename ValueT>
class BinaryNode {

	enum class NodeType {
		LEFT,
		RIGHT,
		ROOT
	};

	ValueT value_{};
	std::shared_ptr<BinaryNode> left_;
	std::shared_ptr<BinaryNode> right_;
	BinaryNode* parent_{ nullptr };
	NodeType type_{ NodeType::ROOT };

public:
	explicit BinaryNode(const ValueT& value) : value_(value) {}

	explicit BinaryNode(const std::shared_ptr<BinaryNode>& left, const std::shared_ptr<BinaryNode>& right) :
			left_(left), right_(right) {
		left_->parent_ = this;
		right_->parent_ = this;
		left_->type_ = NodeType::LEFT;
		right_->type_ = NodeType::RIGHT;
	}

	explicit BinaryNode(const BinaryNode& node) {
		value_ = node.value_;
		if (node.hasLeft()) {
			setLeft(std::make_shared<BinaryNode>(*node.left_));
		}
		if (node.hasRight()) {
			setRight(std::make_shared<BinaryNode>(*node.right_));
		}
	}

	bool isLeaf() const {
		return !hasLeft() && !hasRight();
	}

	bool hasParent() const {
		return type_ != NodeType::ROOT;
	}

	bool hasLeft() const {
		return left_.operator bool();
	}

	bool hasRight() const {
		return right_.operator bool();
	}

	void setValue(int64_t value) {
		value_ = value;
	}

	void setLeft(const std::shared_ptr<BinaryNode>& left) {
		left_ = left;
		left_->parent_ = this;
		left_->type_ = NodeType::LEFT;
	}

	void setRight(const std::shared_ptr<BinaryNode>& right) {
		right_ = right;
		right_->parent_ = this;
		right_->type_ = NodeType::RIGHT;
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

	const ValueT& value() const {
		return value_;
	}

	ValueT& value() {
		return value_;
	}

	void removeChildren() {
		left_.reset();
		right_.reset();
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
			right_->transformRightmostLeaf(func);
		}
		else {
			left_->transformRightmostLeaf(func);
		}
	}

	void transformLeftmostLeafRightBranch(std::function<void(BinaryNode&)> func) {
		if (!hasParent()) {
			return;
		}
		if (type_ == NodeType::RIGHT) {
			parent_->transformLeftmostLeafRightBranch(func);
		}
		else {
			parent_->right().transformLeftmostLeaf(func);
		}
	}

	void transformRightmostLeafLeftBranch(std::function<void(BinaryNode&)> func) {
		if (!hasParent()) {
			return;
		}
		if (type_ == NodeType::LEFT) {
			parent_->transformRightmostLeafLeftBranch(func);
		}
		else {
			parent_->left().transformRightmostLeaf(func);
		}
	}

	std::shared_ptr<BinaryNode> detachLeft() {
		auto detached = left_;
		left_.reset();
		detached->parent_ = nullptr;
		detached->type_ = NodeType::ROOT;
		return detached;
	}

	std::shared_ptr<BinaryNode> detachRight() {
		auto detached = right_;
		right_.reset();
		detached->parent_ = nullptr;
		detached->type_ = NodeType::ROOT;
		return detached;
	}

	template<typename T>
	T reduceLeafs(std::function<T(const T&, const T&)> func) const {
		if (isLeaf()) {
			return value_;
		}
		if (hasLeft() && hasRight()) {
			return func(left_->reduceLeafs(func), right_->reduceLeafs(func));
		}
		if (hasLeft()) {
			return left_->reduceLeafs(func);
		}
		return right_->reduceLeafs(func);
	}

	std::string toString(char braceLeft, char braceRight) const {
		if (isLeaf()) {
			return std::to_string(value_);
		}
		if (hasRight() && hasLeft()) {
			return braceLeft + left_->toString(braceLeft, braceRight) + ',' + right_->toString(braceLeft, braceRight) + braceRight;
		}
		if (hasLeft()) {
			return braceLeft + left_->toString(braceLeft, braceRight) + braceRight;
		}
		return braceLeft + right_->toString(braceLeft, braceRight) + braceRight;
	}

};

} // namespace