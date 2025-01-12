#pragma once

#include <cassert>
#include <optional>
#include <random>
#include <utility>

namespace nono {

namespace ordered_map_node {

template <class Key, class Value>
struct Node {
    Node() = default;
    Node(Key key, Value value): key(key), value(value) {}
    ~Node() {
        if (left) delete left;
        if (right) delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;
    Key key;
    Value value;
};

template <class Key, class Value>
using NodePtr = Node<Key, Value>*;

std::mt19937 rng(std::random_device{}());

///  # empty()
///  whether S is empty
///  O(1)
template <class Key, class Value>
bool empty(NodePtr<Key, Value> root) {
    return !root;
}

///  # size()
///  |S|
///  O(1)
template <class Key, class Value>
int size(NodePtr<Key, Value> root) {
    return root ? root->size : 0;
}

///  # update(root)
///  update size of S
template <class Key, class Value>
void update(NodePtr<Key, Value> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

///  # merge(lhs, rhs)
///  return root of (lhs + rhs)
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> merge(NodePtr<Key, Value> lhs, NodePtr<Key, Value> rhs) {
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
template <class Key, class Value>
std::pair<NodePtr<Key, Value>, NodePtr<Key, Value>> split_by_size(NodePtr<Key, Value> root, int k) {
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
template <class Key, class Value>
std::pair<NodePtr<Key, Value>, NodePtr<Key, Value>> split_by_key(NodePtr<Key, Value> root, Key key) {
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

///  # set(root, key)
///  return root of (S or {key})
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> set(NodePtr<Key, Value> root, Key key, Value value) {
    auto node = find_by_key(root, key);
    if (node) {
        node->value = value;
        return root;
    } else {
        auto [lhs, rhs] = split_by_key(root, key);
        return merge(lhs, merge(new Node<Key, Value>(key, value), rhs));
    }
}

///  # erase(root, key)
///  return root of (S - {key})
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> erase(NodePtr<Key, Value> root, Key key) {
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
template <class Key, class Value>
NodePtr<Key, Value> find_by_size(NodePtr<Key, Value> root, int k) {
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
template <class Key, class Value>
NodePtr<Key, Value> find_by_key(NodePtr<Key, Value> root, Key key) {
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
template <class Key, class Value>
bool contains(NodePtr<Key, Value> root, Key key) {
    return find_by_key(root, key);
}

///  # rank(root, key)
///  return |{ x in S | x < key }|
///  O(log n)
template <class Key, class Value>
int rank(NodePtr<Key, Value> root, Key key) {
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
template <class Key, class Value>
NodePtr<Key, Value> min(NodePtr<Key, Value> root) {
    while (root->left) root = root->left;
    return root;
}

///  # max(root)
///  return max node
///  if not exist, return nullptr
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> max(NodePtr<Key, Value> root) {
    while (root->right) root = root->right;
    return root;
}

///  # kth(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> kth(NodePtr<Key, Value> root, int k) {
    return find_by_size(root, k);
}

///  # successor(node)
///  return next value
///  if not exist, return nullptr
///  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> successor(NodePtr<Key, Value> root, Key key) {
    NodePtr<Key, Value> result = nullptr;
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
template <class Key, class Value>
NodePtr<Key, Value> predecessor(NodePtr<Key, Value> root, Key key) {
    NodePtr<Key, Value> result = nullptr;
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

}  //  namespace ordered_map_node

///  # OrderedMap
///  RBST
///  ordered_map_nodeのラッパー
template <class Key, class Value>
class OrderedMap {
  public:
    OrderedMap() {}
    ~OrderedMap() {
        if (root_) delete root_;
    }

    ///  # set(key, value)
    ///  S[key] <= value
    ///  O(log n)
    void set(Key key, Value value) {
        root_ = ordered_map_node::set(root_, key, value);
    }

    ///  # get(key)
    ///  return S[key]
    ///  if key is not in S, fail
    ///  O(log n)
    Value get(Key key) {
        auto node = ordered_map_node::find_by_key(root_, key);
        assert(node);
        return node->value;
    }

    ///  # erase(key)
    ///  S <= S - key
    ///  if key is not in S, do nothing
    ///  O(log n)
    void erase(Key key) {
        root_ = ordered_map_node::erase(root_, key);
    }

    ///  # empty()
    ///  whether S is empty
    ///  O(1)
    bool empty() {
        return ordered_map_node::empty(root_);
    }

    ///  # size()
    ///  |S|
    ///  O(1)
    int size() {
        return ordered_map_node::size(root_);
    }

    ///  # min()
    ///  min(S)
    ///  O(log n)
    std::pair<Key, Value> min() {
        assert(!empty());
        auto node = ordered_map_node::min(root_);
        return {node->key, node->value};
    }

    ///  # max()
    ///  max(S)
    ///  O(log n)
    std::pair<Key, Value> max() {
        assert(!empty());
        auto node = ordered_map_node::max(root_);
        return {node->key, node->value};
    }

    ///  # kth(k)
    ///  k番目に小さい値
    ///  0-index
    ///  O(log n)
    std::pair<Key, Value> kth(int k) {
        assert(0 <= k && k < size());
        return ordered_map_node::kth(root_, k)->key;
    }

    ///  # contains(key)
    ///  whether key in S
    ///  O(log n)
    bool contains(Key key) {
        return ordered_map_node::contains(root_, key);
    }

    ///  # rank(key)
    ///  |{ (a, b) in S | a < key }|
    ///  O(log n)
    int rank(Key key) {
        return ordered_map_node::rank(root_, key);
    }

    ///  # successor(key)
    ///  return next value
    ///  if not exist, return std::optional
    ///  O(log n)
    std::optional<std::pair<Key, Value>> successor(Key key) {
        auto node = ordered_map_node::successor(root_, key);
        return node ? std::optional<std::pair<Key, Value>>({node->key, node->value}) : std::nullopt;
    }

    ///  # predecessor(key)
    ///  return prev value
    ///  if not exist, return std::optional
    ///  O(log n)
    std::optional<std::pair<Key, Value>> predecessor(Key key) {
        auto node = ordered_map_node::predecessor(root_, key);
        return node ? std::optional<std::pair<Key, Value>>({node->key, node->value}) : std::nullopt;
    }

  private:
    ordered_map_node::NodePtr<Key, Value> root_ = nullptr;
};

}  //  namespace nono