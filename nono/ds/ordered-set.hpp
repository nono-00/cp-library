#pragma once

#include <cassert>
#include <optional>
#include <utility>

#include "./rbst.hpp"

namespace nono {

namespace ordered_set_internal {

template <class T>
struct Node {
    Node() = default;
    Node(T key): key(key) {}
    ~Node() {
        delete left;
        delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;
    T key;
};

template <class T>
using NodePtr = Node<T>*;

//  # empty(root)
//  Sが空か
//  O(1)
template <class T>
bool empty(NodePtr<T> root) {
    return !root;
}

//  # size(root)
//  |S|
//  O(1)
template <class T>
int size(NodePtr<T> root) {
    return root ? root->size : 0;
}

//  # update(root)
//  Sのsizeを更新する
template <class T>
void update(NodePtr<T> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

//  # push(root)
//  Key比較のみで完結するため伝播すべき遅延情報は無い（rbst_internal::merge/split_by_sizeから
//  ADLで呼ばれるためのno-op）
template <class T>
void push(NodePtr<T>) {}

//  # merge(lhs, rhs)
//  lhs + rhs の根を返す
//  O(log n)
template <class T>
NodePtr<T> merge(NodePtr<T> lhs, NodePtr<T> rhs) {
    return rbst_internal::merge(lhs, rhs);
}

//  # split_by_size(root, k)
//  (S[:k] の根, S[k:] の根) を返す
//  size(S) < k なら (S, 空) を返す
//  O(log n)
template <class T>
std::pair<NodePtr<T>, NodePtr<T>> split_by_size(NodePtr<T> root, int k) {
    return rbst_internal::split_by_size(root, k);
}

//  # split_by_key(root, key)
//  return ({ x | x < key }, { x | key <= x })
//  O(log n)
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

//  # insert(root, key)
//  S or {key} の根を返す
//  O(log n)
template <class T>
NodePtr<T> insert(NodePtr<T> root, T key) {
    if (contains(root, key)) return root;
    auto [lhs, rhs] = split_by_key(root, key);
    return merge(lhs, merge(new Node<T>(key), rhs));
}

//  # erase(root, key)
//  S - {key} の根を返す
//  O(log n)
template <class T>
NodePtr<T> erase(NodePtr<T> root, T key) {
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
template <class T>
NodePtr<T> find_by_size(NodePtr<T> root, int k) {
    return rbst_internal::find_by_size(root, k);
}

//  # find_by_key(root, key)
//  keyを持つノードを返す
//  存在しなければnullptrを返す
//  O(log n)
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

//  # contains(root, key)
//  keyがSに含まれるか
//  O(log n)
template <class T>
bool contains(NodePtr<T> root, T key) {
    return find_by_key(root, key);
}

//  # rank(root, key)
//  return |{ x in S | x < key }|
//  O(log n)
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

//  # min(root)
//  最小の要素のノードを返す
//  存在しなければnullptrを返す
//  O(log n)
template <class T>
NodePtr<T> min(NodePtr<T> root) {
    while (root->left) root = root->left;
    return root;
}

//  # max(root)
//  最大の要素のノードを返す
//  存在しなければnullptrを返す
//  O(log n)
template <class T>
NodePtr<T> max(NodePtr<T> root) {
    while (root->right) root = root->right;
    return root;
}

//  # kth(root, k)
//  return S[k]
//  存在しなければnullptrを返す
//  O(log n)
template <class T>
NodePtr<T> kth(NodePtr<T> root, int k) {
    return find_by_size(root, k);
}

//  # successor(root, key)
//  keyより大きい最小の要素を返す
//  存在しなければnullptrを返す
//  O(log n)
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

//  # predecessor(root, key)
//  keyより小さい最大の要素を返す
//  存在しなければnullptrを返す
//  O(log n)
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

}  //  namespace ordered_set_internal

//  # OrderedSet
//  順序付き集合。RBSTで実装している
//  ordered_set_internalのラッパー
template <class T>
class OrderedSet {
  public:
    OrderedSet() {}
    ~OrderedSet() {
        delete root_;
    }

    //  # insert(key)
    //  S <- S or {key}
    //  keyがSにあれば何もしない
    //  O(log n)
    void insert(T key) {
        root_ = ordered_set_internal::insert(root_, key);
    }

    //  # erase(key)
    //  S <- S - {key}
    //  O(log n)
    void erase(T key) {
        root_ = ordered_set_internal::erase(root_, key);
    }

    //  # empty()
    //  Sが空か
    //  O(1)
    bool empty() {
        return ordered_set_internal::empty(root_);
    }

    //  # size()
    //  |S|
    //  O(1)
    int size() {
        return ordered_set_internal::size(root_);
    }

    //  # min()
    //  min(S)
    //  空だと壊れる
    //  O(log n)
    T min() {
        assert(!empty());
        return ordered_set_internal::min(root_)->key;
    }

    //  # max()
    //  max(S)
    //  空だと壊れる
    //  O(log n)
    T max() {
        assert(!empty());
        return ordered_set_internal::max(root_)->key;
    }

    //  # kth(k)
    //  S[k]
    //  0-index
    //  0 <= k < size() でないと壊れる
    //  O(log n)
    T kth(int k) {
        assert(0 <= k && k < size());
        return ordered_set_internal::kth(root_, k)->key;
    }

    //  # contains(key)
    //  keyがSに含まれるか
    //  O(log n)
    bool contains(T key) {
        return ordered_set_internal::contains(root_, key);
    }

    //  # rank(key)
    //  |{ x in S | x < key }|
    //  O(log n)
    int rank(T key) {
        return ordered_set_internal::rank(root_, key);
    }

    //  # successor(key)
    //  keyより大きい最小の要素を返す
    //  存在しなければnulloptを返す
    //  O(log n)
    std::optional<T> successor(T key) {
        auto node = ordered_set_internal::successor(root_, key);
        return node ? std::optional<T>(node->key) : std::nullopt;
    }

    //  # predecessor(key)
    //  keyより小さい最大の要素を返す
    //  存在しなければnulloptを返す
    //  O(log n)
    std::optional<T> predecessor(T key) {
        auto node = ordered_set_internal::predecessor(root_, key);
        return node ? std::optional<T>(node->key) : std::nullopt;
    }

  private:
    ordered_set_internal::NodePtr<T> root_ = nullptr;
};

}  //  namespace nono
