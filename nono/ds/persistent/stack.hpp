#pragma once

#include <cassert>
#include <memory>

namespace nono {

namespace persistent {

namespace stack_internal {

template <class T>
struct Node;

template <class T>
using NodePtr = std::shared_ptr<Node<const T>>;

template <class T>
struct Node {
    Node(T element, NodePtr<T> parent): element(element), parent(parent), size(parent ? parent->size + 1 : 1) {}
    T element;
    NodePtr<T> parent;
    int size;
};

template <class T>
NodePtr<T> push(const NodePtr<T> node, T element) {
    return std::make_shared<Node<const T>>(element, node);
}

template <class T>
NodePtr<T> pop(const NodePtr<T> node) {
    assert(node);
    return node->parent;
}

template <class T>
bool empty(const NodePtr<T> node) {
    return !node;
}

template <class T>
T top(const NodePtr<T> node) {
    assert(node);
    return node->element;
}

template <class T>
int size(const NodePtr<T> node) {
    return node ? node->size : 0;
}

}  //  namespace stack_internal

//  # Stack
//  永続スタック。push / pop は自分を書き換えず、新しいスタックを返す。古い版もそのまま使える
template <class T>
class Stack {
  public:
    Stack(): node_(nullptr) {}
    [[nodiscard]] explicit Stack(const stack_internal::NodePtr<T> node): node_(node) {}

    //  # push(element)
    //  elementを積んだスタックを返す
    //  O(1)
    [[nodiscard]] Stack push(T element) const {
        return Stack{std::make_shared<stack_internal::Node<const T>>(element, node_)};
    }

    //  # pop()
    //  一番上の要素を取り除いたスタックを返す
    //  空だと壊れる
    //  O(1)
    [[nodiscard]] Stack pop() const {
        assert(!stack_internal::empty(node_));
        return Stack{node_->parent};
    }

    //  # empty()
    //  空か
    bool empty() const {
        return stack_internal::empty(node_);
    }

    //  # top()
    //  一番上の要素を返す
    //  空だと壊れる
    T top() const {
        assert(!stack_internal::empty(node_));
        return stack_internal::top(node_);
    }

    //  # size()
    //  要素数
    //  O(1)
    int size() const {
        return stack_internal::size(node_);
    }

  private:
    stack_internal::NodePtr<T> node_;
};

}  //  namespace persistent

}  //  namespace nono
