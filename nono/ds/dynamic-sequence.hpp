#pragma once

#include <cassert>
#include <optional>
#include <random>
#include <utility>

#include "../structure/act-monoid.hpp"

namespace nono {

namespace dynamic_sequence_node {

template <class M>
struct Node {
    Node(M::Value element): element(element), total(element), act(std::nullopt) {}

    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;

    act_monoid::Rev<M>::Value element, total;
    std::optional<typename M::Act> act;
    bool rev = false;
};

template <class M>
using NodePtr = Node<M>*;

std::mt19937 rng(std::random_device{}());

//  -- CHANGE --

///  # empty()
///  whether S is empty
///  O(1)
template <class M>
bool empty(NodePtr<M> root) {
    return !root;
}

///  # size()
///  |S|
///  O(1)
template <class M>
int size(NodePtr<M> root) {
    return root ? root->size : 0;
}

template <class M>
act_monoid::Rev<M>::Value prod(NodePtr<M> root) {
    return root ? root->total : M::e();
}

///  # update(root)
///  update size of S
template <class M>
void update(NodePtr<M> root) {
    using Rev = act_monoid::Rev<M>;
    root->size = 1 + size(root->left) + size(root->right);
    root->total = Rev::op(Rev::op(prod(root->left), root->element), prod(root->right));
}

template <class M>
void apply(NodePtr<M> root, typename M::Act act) {
    using Rev = act_monoid::Rev<M>;
    if (!root) return;
    root->element = Rev::mapping(act, root->element);
    root->total = Rev::mapping(act, root->total);
    root->act = Rev::composition(act, root->act ? *root->act : M::id());
}

template <class M>
void reverse(NodePtr<M> root) {
    if (!root) return;
    root->rev ^= true;
    std::swap(root->total.ord, root->total.rev);
    std::swap(root->left, root->right);
}

template <class M>
void push(NodePtr<M> root) {
    if (root->rev) {
        reverse(root->left);
        reverse(root->right);
        root->rev = false;
    }
    if (root->act) {
        apply(root->left, *root->act);
        apply(root->right, *root->act);
        root->act = std::nullopt;
    }
}

///  # merge(lhs, rhs)
///  return root of (lhs + rhs)
///  O(log n)
template <class M>
NodePtr<M> merge(NodePtr<M> lhs, NodePtr<M> rhs) {
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (((long long)rng() * (lhs->size + rhs->size)) >> 32 < lhs->size) {
        push(lhs);
        lhs->right = merge(lhs->right, rhs);
        update(lhs);
        return lhs;
    } else {
        push(rhs);
        rhs->left = merge(lhs, rhs->left);
        update(rhs);
        return rhs;
    }
}

///  # split by size(root, k)
///  return (root of S[:k], root of S[k:])
///  if size(S) < k, return {S, empty set}
///  O(log n)
template <class M>
std::pair<NodePtr<M>, NodePtr<M>> split_by_size(NodePtr<M> root, int k) {
    if (!root) return {nullptr, nullptr};
    push(root);
    if (size(root->left) >= k) {
        auto [lhs, rhs] = split_by_size(root->left, k);
        root->left = rhs;
        update(root);
        return {lhs, root};
    } else {
        auto [lhs, rhs] = split_by_size(root->right, k - size(root->left) - 1);
        root->right = lhs;
        update(root);
        return {root, rhs};
    }
}

///  # insert(root, index, element)
///  return data[:index] + element + data[index:]
///  O(log n)
template <class M>
NodePtr<M> insert(NodePtr<M> root, int index, typename M::Value element) {
    assert(0 <= index && index <= size(root));
    auto [lhs, rhs] = split_by_size(root, index);
    return merge(lhs, merge(new Node<M>(element), rhs));
}

///  # erase(root, index)
///  return data[:index] + data[index + 1:]
///  O(log n)
template <class M>
NodePtr<M> erase(NodePtr<M> root, int index) {
    assert(0 <= index && index < size(root));
    auto [lhs, temp] = split_by_size(root, index);
    auto [mhs, rhs] = split_by_size(temp, 1);
    delete mhs;
    return merge(lhs, rhs);
}

//  -- SEARCH --

///  # find by size(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class M>
NodePtr<M> find_by_size(NodePtr<M> root, int k) {
    while (root) {
        push(root);
        if (size(root->left) > k) {
            root = root->left;
        } else if (size(root->left) == k) {
            return root;
        } else {
            k -= size(root->left) + 1;
            root = root->right;
        }
    }
    return nullptr;
}

///  # kth(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class M>
NodePtr<M> kth(NodePtr<M> root, int k) {
    return find_by_size(root, k);
}

}  //  namespace dynamic_sequence_node

template <class M>
class DynamicSequence {
    using T = M::Value;
    using F = M::Act;

  public:
    DynamicSequence() {}

    void insert(int index, T element) {
        root_ = dynamic_sequence_node::insert(root_, index, element);
    }

    void erase(int index) {
        root_ = dynamic_sequence_node::erase(root_, index);
    }

    bool empty() {
        return dynamic_sequence_node::empty(root_);
    }

    int size() {
        return dynamic_sequence_node::size(root_);
    }

    T get(int index) {
        assert(0 <= index && index < size());
        return dynamic_sequence_node::find_by_size(root_, index)->element.ord;
    }

    void push_back(T element) {
        insert(size(), element);
    }

    void pop_back() {
        assert(!empty());
        erase(size() - 1);
    }

    void push_front(T element) {
        insert(0, element);
    }

    void pop_front() {
        assert(!empty());
        erase(0);
    }

    void set(int index, T element) {
        assert(0 <= index && index < size());
        erase(index);
        insert(index, element);
    }

    void reverse(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_node::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_node::split_by_size(temp, right - left);
        dynamic_sequence_node::reverse(mhs);
        root_ = dynamic_sequence_node::merge(dynamic_sequence_node::merge(lhs, mhs), rhs);
    }

    void apply(int left, int right, F act) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_node::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_node::split_by_size(temp, right - left);
        dynamic_sequence_node::apply(mhs, act);
        root_ = dynamic_sequence_node::merge(dynamic_sequence_node::merge(lhs, mhs), rhs);
    }

    T prod(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_node::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_node::split_by_size(temp, right - left);
        auto ans = dynamic_sequence_node::prod(mhs);
        root_ = dynamic_sequence_node::merge(dynamic_sequence_node::merge(lhs, mhs), rhs);
        return ans.ord;
    }

  private:
    dynamic_sequence_node::NodePtr<M> root_ = nullptr;
};

}  //  namespace nono