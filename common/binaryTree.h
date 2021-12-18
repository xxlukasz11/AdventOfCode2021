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
	explicit BinaryNode(const ValueT& value);
	explicit BinaryNode(const std::shared_ptr<BinaryNode>& left, const std::shared_ptr<BinaryNode>& right);
	explicit BinaryNode(const BinaryNode& node);
	std::shared_ptr<BinaryNode> clone() const;

	bool isLeaf() const;
	bool hasParent() const;
	bool hasLeft() const;
	bool hasRight() const;

	void setValue(int64_t value);
	void setLeft(const std::shared_ptr<BinaryNode>& left);
	void setRight(const std::shared_ptr<BinaryNode>& right);

	BinaryNode& parent();
	const BinaryNode& parent() const;
	BinaryNode& left();
	const BinaryNode& left() const;
	BinaryNode& right();
	const BinaryNode& right() const;
	ValueT& value();
	const ValueT& value() const;

	void transformLeftmostLeaf(std::function<void(BinaryNode&)> func);
	void transformRightmostLeaf(std::function<void(BinaryNode&)> func);
	void transformLeftmostLeafRightBranch(std::function<void(BinaryNode&)> func);
	void transformRightmostLeafLeftBranch(std::function<void(BinaryNode&)> func);

	template<typename T>
	T reduceLeafs(std::function<T(const T&, const T&)> func) const;

	std::shared_ptr<BinaryNode> detachLeft();
	std::shared_ptr<BinaryNode> detachRight();
	void removeChildren();

	std::string toString(char braceLeft, char braceRight) const;

};

template<typename ValueT>
inline BinaryNode<ValueT>::BinaryNode(const ValueT& value) : value_(value) {
}

template<typename ValueT>
inline BinaryNode<ValueT>::BinaryNode(const std::shared_ptr<BinaryNode>& left, const std::shared_ptr<BinaryNode>& right) :
	left_(left), right_(right) {
	left_->parent_ = this;
	right_->parent_ = this;
	left_->type_ = NodeType::LEFT;
	right_->type_ = NodeType::RIGHT;
}

template<typename ValueT>
inline BinaryNode<ValueT>::BinaryNode(const BinaryNode& node) {
	value_ = node.value_;
	if (node.hasLeft()) {
		setLeft(std::make_shared<BinaryNode>(*node.left_));
	}
	if (node.hasRight()) {
		setRight(std::make_shared<BinaryNode>(*node.right_));
	}
}

template<typename ValueT>
inline std::shared_ptr<BinaryNode<ValueT>> BinaryNode<ValueT>::clone() const {
	auto node = std::make_shared<BinaryNode>(value_);
	if (hasLeft()) {
		node->setLeft(std::make_shared<BinaryNode>(*left_));
	}
	if (hasRight()) {
		node->setRight(std::make_shared<BinaryNode>(*right_));
	}
	return node;
}

template<typename ValueT>
inline bool BinaryNode<ValueT>::isLeaf() const {
	return !hasLeft() && !hasRight();
}

template<typename ValueT>
inline bool BinaryNode<ValueT>::hasParent() const {
	return type_ != NodeType::ROOT;
}

template<typename ValueT>
inline bool BinaryNode<ValueT>::hasLeft() const {
	return left_.operator bool();
}

template<typename ValueT>
inline bool BinaryNode<ValueT>::hasRight() const {
	return right_.operator bool();
}

template<typename ValueT>
inline void BinaryNode<ValueT>::setValue(int64_t value) {
	value_ = value;
}

template<typename ValueT>
inline void BinaryNode<ValueT>::setLeft(const std::shared_ptr<BinaryNode>& left) {
	left_ = left;
	left_->parent_ = this;
	left_->type_ = NodeType::LEFT;
}

template<typename ValueT>
inline void BinaryNode<ValueT>::setRight(const std::shared_ptr<BinaryNode>& right) {
	right_ = right;
	right_->parent_ = this;
	right_->type_ = NodeType::RIGHT;
}

template<typename ValueT>
inline BinaryNode<ValueT>& BinaryNode<ValueT>::parent() {
	return *parent_;
}

template<typename ValueT>
inline const BinaryNode<ValueT>& BinaryNode<ValueT>::parent() const {
	return *parent_;
}

template<typename ValueT>
inline BinaryNode<ValueT>& BinaryNode<ValueT>::left() {
	return *left_;
}

template<typename ValueT>
inline const BinaryNode<ValueT>& BinaryNode<ValueT>::left() const {
	return *left_;
}

template<typename ValueT>
inline BinaryNode<ValueT>& BinaryNode<ValueT>::right() {
	return *right_;
}

template<typename ValueT>
inline const BinaryNode<ValueT>& BinaryNode<ValueT>::right() const {
	return *right_;
}

template<typename ValueT>
inline ValueT& BinaryNode<ValueT>::value() {
	return value_;
}

template<typename ValueT>
inline const ValueT& BinaryNode<ValueT>::value() const {
	return value_;
}

template<typename ValueT>
inline void BinaryNode<ValueT>::transformLeftmostLeaf(std::function<void(BinaryNode&)> func) {
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

template<typename ValueT>
inline void BinaryNode<ValueT>::transformRightmostLeaf(std::function<void(BinaryNode&)> func) {
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

template<typename ValueT>
inline void BinaryNode<ValueT>::transformLeftmostLeafRightBranch(std::function<void(BinaryNode&)> func) {
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

template<typename ValueT>
inline void BinaryNode<ValueT>::transformRightmostLeafLeftBranch(std::function<void(BinaryNode&)> func) {
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

template<typename ValueT>
template<typename T>
inline T BinaryNode<ValueT>::reduceLeafs(std::function<T(const T&, const T&)> func) const {
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

template<typename ValueT>
inline std::shared_ptr<BinaryNode<ValueT>> BinaryNode<ValueT>::detachLeft() {
	auto detached = left_;
	left_.reset();
	detached->parent_ = nullptr;
	detached->type_ = NodeType::ROOT;
	return detached;
}

template<typename ValueT>
inline std::shared_ptr<BinaryNode<ValueT>> BinaryNode<ValueT>::detachRight() {
	auto detached = right_;
	right_.reset();
	detached->parent_ = nullptr;
	detached->type_ = NodeType::ROOT;
	return detached;
}

template<typename ValueT>
inline void BinaryNode<ValueT>::removeChildren() {
	left_->type_ = NodeType::ROOT;
	right_->type_ = NodeType::ROOT;
	left_.reset();
	right_.reset();
}

template<typename ValueT>
inline std::string BinaryNode<ValueT>::toString(char braceLeft, char braceRight) const {
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

} // namespace