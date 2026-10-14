#pragma once

#include <cassert>
#include <optional>
#include <utility>

#include "./rbst.hpp"

namespace nono {

namespace ordered_map_internal {

template <class Key, class Value>
struct Node {
    Node() = default;
    Node(Key key, Value value): key(key), value(value) {}
    ~Node() {
        delete left;
        delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;
    Key key;
    Value value;
};

template <class Key, class Value>
using NodePtr = Node<Key, Value>*;

//  # empty(root)
//  Sが空か
//  O(1)
template <class Key, class Value>
bool empty(NodePtr<Key, Value> root) {
    return !root;
}

//  # size(root)
//  |S|
//  O(1)
template <class Key, class Value>
int size(NodePtr<Key, Value> root) {
    return root ? root->size : 0;
}

//  # update(root)
//  Sのsizeを更新する
template <class Key, class Value>
void update(NodePtr<Key, Value> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

//  # push(root)
//  Key比較のみで完結するため伝播すべき遅延情報は無い（rbst_internal::merge/split_by_sizeから
//  ADLで呼ばれるためのno-op）
template <class Key, class Value>
void push(NodePtr<Key, Value>) {}

//  # merge(lhs, rhs)
//  lhs + rhs の根を返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> merge(NodePtr<Key, Value> lhs, NodePtr<Key, Value> rhs) {
    return rbst_internal::merge(lhs, rhs);
}

//  # split_by_size(root, k)
//  (S[:k] の根, S[k:] の根) を返す
//  size(S) < k なら (S, 空) を返す
//  O(log n)
template <class Key, class Value>
std::pair<NodePtr<Key, Value>, NodePtr<Key, Value>> split_by_size(NodePtr<Key, Value> root, int k) {
    return rbst_internal::split_by_size(root, k);
}

//  # split_by_key(root, key)
//  return ({ x | x < key }, { x | key <= x })
//  O(log n)
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

//  # set(root, key)
//  S or {key} の根を返す
//  O(log n)
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

//  # erase(root, key)
//  S - {key} の根を返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> erase(NodePtr<Key, Value> root, Key key) {
    if (!contains(root, key)) return root;
    auto [lhs, temp] = split_by_key(root, key);
    auto [mhs, rhs] = split_by_size(temp, 1);
    delete mhs;
    return merge(lhs, rhs);
}

//  # find_by_size(root, k)
//  return S[k]
//  存在しなければnullptrを返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> find_by_size(NodePtr<Key, Value> root, int k) {
    return rbst_internal::find_by_size(root, k);
}

//  # find_by_key(root, key)
//  keyを持つノードを返す
//  存在しなければnullptrを返す
//  O(log n)
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

//  # contains(root, key)
//  keyがSに含まれるか
//  O(log n)
template <class Key, class Value>
bool contains(NodePtr<Key, Value> root, Key key) {
    return find_by_key(root, key);
}

//  # rank(root, key)
//  return |{ x in S | x < key }|
//  O(log n)
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

//  # min(root)
//  最小の要素のノードを返す
//  存在しなければnullptrを返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> min(NodePtr<Key, Value> root) {
    while (root->left) root = root->left;
    return root;
}

//  # max(root)
//  最大の要素のノードを返す
//  存在しなければnullptrを返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> max(NodePtr<Key, Value> root) {
    while (root->right) root = root->right;
    return root;
}

//  # kth(root, k)
//  return S[k]
//  存在しなければnullptrを返す
//  O(log n)
template <class Key, class Value>
NodePtr<Key, Value> kth(NodePtr<Key, Value> root, int k) {
    return find_by_size(root, k);
}

//  # successor(root, key)
//  keyより大きい最小の要素を返す
//  存在しなければnullptrを返す
//  O(log n)
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

//  # predecessor(root, key)
//  keyより小さい最大の要素を返す
//  存在しなければnullptrを返す
//  O(log n)
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

}  //  namespace ordered_map_internal

//  # OrderedMap
//  順序付き連想配列。RBSTで実装している
//  ordered_map_internalのラッパー
template <class Key, class Value>
class OrderedMap {
  public:
    OrderedMap() {}
    ~OrderedMap() {
        delete root_;
    }

    //  # set(key, value)
    //  S[key] <= value
    //  O(log n)
    void set(Key key, Value value) {
        root_ = ordered_map_internal::set(root_, key, value);
    }

    //  # get(key)
    //  return S[key]
    //  keyがSに無いと壊れる
    //  O(log n)
    Value get(Key key) {
        auto node = ordered_map_internal::find_by_key(root_, key);
        assert(node);
        return node->value;
    }

    //  # erase(key)
    //  S <= S - key
    //  keyがSに無ければ何もしない
    //  O(log n)
    void erase(Key key) {
        root_ = ordered_map_internal::erase(root_, key);
    }

    //  # empty()
    //  Sが空か
    //  O(1)
    bool empty() {
        return ordered_map_internal::empty(root_);
    }

    //  # size()
    //  |S|
    //  O(1)
    int size() {
        return ordered_map_internal::size(root_);
    }

    //  # min()
    //  キーが最小の (key, value)
    //  空だと壊れる
    //  O(log n)
    std::pair<Key, Value> min() {
        assert(!empty());
        auto node = ordered_map_internal::min(root_);
        return {node->key, node->value};
    }

    //  # max()
    //  キーが最大の (key, value)
    //  空だと壊れる
    //  O(log n)
    std::pair<Key, Value> max() {
        assert(!empty());
        auto node = ordered_map_internal::max(root_);
        return {node->key, node->value};
    }

    //  # kth(k)
    //  キーがk番目に小さい (key, value)
    //  0-index
    //  0 <= k < size() でないと壊れる
    //  O(log n)
    std::pair<Key, Value> kth(int k) {
        assert(0 <= k && k < size());
        auto node = ordered_map_internal::kth(root_, k);
        return {node->key, node->value};
    }

    //  # contains(key)
    //  keyがSに含まれるか
    //  O(log n)
    bool contains(Key key) {
        return ordered_map_internal::contains(root_, key);
    }

    //  # rank(key)
    //  |{ (a, b) in S | a < key }|
    //  O(log n)
    int rank(Key key) {
        return ordered_map_internal::rank(root_, key);
    }

    //  # successor(key)
    //  keyより大きい最小の要素を返す
    //  存在しなければnulloptを返す
    //  O(log n)
    std::optional<std::pair<Key, Value>> successor(Key key) {
        auto node = ordered_map_internal::successor(root_, key);
        return node ? std::optional<std::pair<Key, Value>>({node->key, node->value}) : std::nullopt;
    }

    //  # predecessor(key)
    //  keyより小さい最大の要素を返す
    //  存在しなければnulloptを返す
    //  O(log n)
    std::optional<std::pair<Key, Value>> predecessor(Key key) {
        auto node = ordered_map_internal::predecessor(root_, key);
        return node ? std::optional<std::pair<Key, Value>>({node->key, node->value}) : std::nullopt;
    }

  private:
    ordered_map_internal::NodePtr<Key, Value> root_ = nullptr;
};

}  //  namespace nono
