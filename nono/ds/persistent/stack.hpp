#pragma once

#include <cassert>
#include <memory>

namespace nono {

namespace persistent {

namespace stack {

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

}  //  namespace stack

template <class T>
class Stack {
  public:
    Stack(): node_(nullptr) {}
    [[nodiscard]] explicit Stack(const stack::NodePtr<T> node): node_(node) {}

    [[nodiscard]] Stack push(T element) const {
        return Stack{std::make_shared<stack::Node<const T>>(element, node_)};
    }

    [[nodiscard]] Stack pop() const {
        assert(!stack::empty(node_));
        return Stack{node_->parent};
    }

    bool empty() const {
        return stack::empty(node_);
    }

    T top() const {
        assert(!stack::empty(node_));
        return stack::top(node_);
    }

    int size() const {
        return stack::size(node_);
    }

  private:
    stack::NodePtr<T> node_;
};

}  //  namespace persistent

}  //  namespace nono