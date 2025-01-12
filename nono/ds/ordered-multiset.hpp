#pragma once

#include <cassert>
#include <optional>
#include <random>
#include <utility>

namespace nono {

namespace ordered_multiset_node {

template <class T>
struct Node {
    Node() = default;
    Node(T key): key(key) {}
    ~Node() {
        if (left) delete left;
        if (right) delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;
    T key;
};

template <class T>
using NodePtr = Node<T>*;

std::mt19937 rng(std::random_device{}());

///  # empty()
///  whether S is empty
///  O(1)
template <class T>
bool empty(NodePtr<T> root) {
    return !root;
}

///  # size()
///  |S|
///  O(1)
template <class T>
int size(NodePtr<T> root) {
    return root ? root->size : 0;
}

///  # update(root)
///  update size of S
template <class T>
void update(NodePtr<T> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

///  # merge(lhs, rhs)
///  return root of (lhs + rhs)
///  O(log n)
template <class T>
NodePtr<T> merge(NodePtr<T> lhs, NodePtr<T> rhs) {
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (((long long)rng() * (lhs->size + rhs->size)) >> 32 < lhs->size) {
        lhs->right = merge(lhs->right, rhs);
        update(lhs);
        return lhs;
    } else {
        rhs->left = merge(lhs, rhs->left);
        update(rhs);
        return rhs;
    }
}

///  # split by size(root, k)
///  return (root of S[:k], root of S[k:])
///  if size(S) < k, return {S, empty set}
///  O(log n)
template <class T>
std::pair<NodePtr<T>, NodePtr<T>> split_by_size(NodePtr<T> root, int k) {
    if (!root) return {nullptr, nullptr};
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

///  # split by key(root, key)
///  return ({ x | x < key }, { x | key <= x })
///  O(log n)
template <class T>
std::pair<NodePtr<T>, NodePtr<T>> split_by_key(NodePtr<T> root, T key) {
    if (!root) return {nullptr, nullptr};
    if (root->key < key) {
        auto [lhs, rhs] = split_by_key(root->right, key);
        root->right = lhs;
        update(root);
        return {root, rhs};
    } else {
        auto [lhs, rhs] = split_by_key(root->left, key);
        root->left = rhs;
        update(root);
        return {lhs, root};
    }
}

///  # insert(root, key)
///  return root of (S or {key})
///  O(log n)
template <class T>
NodePtr<T> insert(NodePtr<T> root, T key) {
    auto [lhs, rhs] = split_by_key(root, key);
    return merge(lhs, merge(new Node<T>(key), rhs));
}

///  # erase(root, key)
///  return root of (S - {key})
///  erase only one element
///  O(log n)
template <class T>
NodePtr<T> erase(NodePtr<T> root, T key) {
    if (!contains(root, key)) return root;
    auto [lhs, temp] = split_by_key(root, key);
    auto [mhs, rhs] = split_by_size(temp, 1);
    delete mhs;
    return merge(lhs, rhs);
}

///  # find by size(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> find_by_size(NodePtr<T> root, int k) {
    while (root) {
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

///  # find by key(root, key)
///  return key node
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> find_by_key(NodePtr<T> root, T key) {
    while (root) {
        if (key < root->key) {
            root = root->left;
        } else if (key > root->key) {
            root = root->right;
        } else {
            return root;
        }
    }
    return nullptr;
}

///  # contains(root, key)
///  return whether key in S
///  O(log n)
template <class T>
bool contains(NodePtr<T> root, T key) {
    return find_by_key(root, key);
}

///  # rank(root, key)
///  return |{ x in S | x < key }|
///  O(log n)
template <class T>
int rank(NodePtr<T> root, T key) {
    int result = 0;
    while (root) {
        if (root->key < key) {
            result += size(root->left) + 1;
            root = root->right;
        } else {
            root = root->left;
        }
    }
    return result;
}

///  # min(root)
///  return min node
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> min(NodePtr<T> root) {
    while (root->left) root = root->left;
    return root;
}

///  # max(root)
///  return max node
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> max(NodePtr<T> root) {
    while (root->right) root = root->right;
    return root;
}

///  # kth(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> kth(NodePtr<T> root, int k) {
    return find_by_size(root, k);
}

///  # successor(node)
///  return next value
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> successor(NodePtr<T> root, T key) {
    NodePtr<T> result = nullptr;
    while (root) {
        if (key < root->key) result = root;
        if (key < root->key) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return result;
}

///  # predecessor(node)
///  return prev value
///  if not exist, return nullptr
///  O(log n)
template <class T>
NodePtr<T> predecessor(NodePtr<T> root, T key) {
    NodePtr<T> result = nullptr;
    while (root) {
        if (root->key < key) result = root;
        if (root->key < key) {
            root = root->right;
        } else {
            root = root->left;
        }
    }
    return result;
}

///  # count(root, key)
///  return num of key in S
///  O(log n)
template <class T>
NodePtr<T> count(NodePtr<T> root, T key) {
    auto node = successor(root, key);
    return node ? rank(root, node->key) : size(root) - rank(root, key);
}

}  //  namespace ordered_multiset_node

///  # OrderedMultiSet
///  RBST
template <class T>
class OrderedMultiSet {
  public:
    OrderedMultiSet() {}
    ~OrderedMultiSet() {
        if (root_) delete root_;
    }

    ///  # insert(key)
    ///  S <- S or {key}
    ///  O(log n)
    void insert(T key) {
        root_ = ordered_multiset_node::insert(root_, key);
    }

    ///  # erase(key)
    ///  S <- S or {key}
    ///  O(log n)
    void erase(T key) {
        root_ = ordered_multiset_node::erase(root_, key);
    }

    ///  # empty()
    ///  whether S is empty
    ///  O(1)
    bool empty() {
        return ordered_multiset_node::empty(root_);
    }

    ///  # size()
    ///  |S|
    ///  O(1)
    int size() {
        return ordered_multiset_node::size(root_);
    }

    ///  # min()
    ///  min(S)
    ///  O(log n)
    T min() {
        assert(!empty());
        return ordered_multiset_node::min(root_)->key;
    }

    ///  # max()
    ///  max(S)
    ///  O(log n)
    T max() {
        assert(!empty());
        return ordered_multiset_node::max(root_)->key;
    }

    ///  # kth(k)
    ///  S[k]
    ///  0-index
    ///  O(log n)
    T kth(int k) {
        assert(0 <= k && k < size());
        return ordered_multiset_node::kth(root_, k)->key;
    }

    ///  # contains(key)
    ///  whether key in S
    ///  O(log n)
    bool contains(T key) {
        return ordered_multiset_node::contains(root_, key);
    }

    ///  # rank(key)
    ///  |{ x in S | x < key }|
    ///  O(log n)
    int rank(T key) {
        return ordered_multiset_node::rank(root_, key);
    }

    ///  # count(key)
    ///  num of key in S
    ///  O(log n)
    int count(T key) {
        return ordered_multiset_node::count(root_, key);
    }

    ///  # successor(key)
    ///  return next value
    ///  if not exist, return std::nullptr
    ///  O(log n)
    std::optional<T> successor(T key) {
        auto node = ordered_multiset_node::successor(root_, key);
        return node ? std::optional<T>(node->key) : std::nullopt;
    }

    ///  # predecessor(key)
    ///  return next value
    ///  if not exist, return std::nullptr
    ///  O(log n)
    std::optional<T> predecessor(T key) {
        auto node = ordered_multiset_node::predecessor(root_, key);
        return node ? std::optional<T>(node->key) : std::nullopt;
    }

  private:
    ordered_multiset_node::NodePtr<T> root_ = nullptr;
};

}  //  namespace nono