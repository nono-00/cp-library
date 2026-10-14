#pragma once

#include <cassert>
#include <limits>
#include <tuple>
#include <utility>

namespace nono {

namespace binary_trie_internal {

template <class T = unsigned, int W = std::numeric_limits<T>::digits>
struct Node {
    Node(): size(0), lazy(0), child{nullptr, nullptr} {}
    ~Node() {
        delete child[0];
        delete child[1];
    }
    void push(int w) {
        assert(w >= 0);
        if (lazy == 0) return;
        if ((lazy >> w) & 1) std::swap(child[0], child[1]);
        if (child[0]) child[0]->lazy ^= lazy;
        if (child[1]) child[1]->lazy ^= lazy;
        lazy = 0;
    }
    void update() {
        size = 0;
        if (child[0]) size += child[0]->size;
        if (child[1]) size += child[1]->size;
    }
    int size;
    T lazy;
    Node* child[2];
};

template <class T, int W>
using NodePtr = Node<T, W>*;

//  valueを1個追加する
template <class T, int W>
NodePtr<T, W> insert(NodePtr<T, W> node, T value, int w = W - 1) {
    if (!node) node = new Node<T, W>();
    if (w >= 0) {
        node->push(w);
        node->child[(value >> w) & 1] = insert(node->child[(value >> w) & 1], value, w - 1);
        node->update();
    } else {
        node->size++;
    }
    return node;
}

//  存在しない場合はなにもしない
template <class T, int W>
NodePtr<T, W> erase(NodePtr<T, W> node, T value, int w = W - 1) {
    if (!node) return nullptr;
    if (w >= 0) {
        node->push(w);
        node->child[(value >> w) & 1] = erase(node->child[(value >> w) & 1], value, w - 1);
        node->update();
    } else {
        node->size--;
    }
    if (node->size == 0) {
        delete node;
        node = nullptr;
    }
    return node;
}

//  not verified
template <class T, int W>
NodePtr<T, W> chain(T value) {
    return insert(nullptr, value);
}

//  not verified
template <class T, int W>
NodePtr<T, W> merge(NodePtr<T, W> lhs, NodePtr<T, W> rhs, int w = W - 1) {
    if (!lhs) return rhs;
    if (!rhs) return lhs;
    if (w >= 0) {
        lhs->push(w);
        rhs->push(w);
        lhs->child[0] = merge(lhs->child[0], rhs->child[0], w - 1);
        lhs->child[1] = merge(lhs->child[1], rhs->child[1], w - 1);
        lhs->update();
    } else {
        assert(!lhs->child[0] && !lhs->child[1]);
        assert(!rhs->child[0] && !rhs->child[1]);
        lhs->size += rhs->size;
    }
    rhs->child = {nullptr, nullptr};
    delete rhs;
    return lhs;
}

//  not verified
//  左k個, 右n - k個
template <class T, int W>
std::pair<NodePtr<T, W>, NodePtr<T, W>> split(NodePtr<T, W> node, int k, int w = W - 1) {
    if (k == 0) return {nullptr, node};
    assert(node && node->size >= k);
    if (node->size == k) return {node, nullptr};
    if (w >= 0) {
        node->push(w);
        if (node->child[0] && node->child[0]->size >= k) {
            NodePtr<T, W> newnode = new Node<T, W>();
            std::tie(newnode->child[0], node->child[0]) = split(node->child[0], k, w - 1);
            newnode->update();
            node->update();
            assert(newnode->size == k);
            return {newnode, node};
        } else {
            NodePtr<T, W> newnode = new Node<T, W>();
            std::tie(node->child[1], newnode->child[1]) =
                split(node->child[1], k - (!node->child[0] ? 0 : node->child[0]->size), w - 1);
            node->update();
            newnode->update();
            assert(node->size == k);
            return {node, newnode};
        }
    } else {
        NodePtr<T, W> newnode = new Node<T, W>();
        newnode->size = k;
        node->size -= k;
        return {newnode, node};
    }
}

//  全ての要素にxor valueする(遅延させる)
template <class T, int W>
NodePtr<T, W> apply_xor(NodePtr<T, W> node, T value) {
    if (node) node->lazy ^= value;
    return node;
}

//  0-index
//  存在しないと壊れる
//  valueを適用した最小値を返す
template <class T, int W>
T kth(NodePtr<T, W> node, int k, T value, int w = W - 1) {
    assert(node && 0 <= k && k < node->size);
    if (w < 0) return T{0};
    node->push(w);
    bool i = (value >> w) & 1;
    if (node->child[i] && k < node->child[i]->size) {
        return kth(node->child[i], k, value, w - 1);
    } else {
        return (T{1} << w) | kth(node->child[i ^ 1], k - (!node->child[i] ? 0 : node->child[i]->size), value, w - 1);
    }
}

//  not verified
//  空だと壊れる
template <class T, int W>
T max(NodePtr<T, W> node, T value, int w = W - 1) {
    assert(node && node->size);
    return kth(node, node->size - 1, value, w);
}

//  空だと壊れる
template <class T, int W>
T min(NodePtr<T, W> node, T value, int w = W - 1) {
    assert(node && node->size);
    return kth(node, 0, value, w);
}

//  valueの個数
template <class T, int W>
int count(NodePtr<T, W> node, T value, int w = W - 1) {
    if (!node) return 0;
    if (w >= 0) {
        node->push(w);
        return count(node->child[(value >> w) & 1], value, w - 1);
    } else {
        return node->size;
    }
}

//  valueより真に小さい値の個数
template <class T, int W>
int count_lt(NodePtr<T, W> node, T value, int w = W - 1) {
    if (!node) return 0;
    if (w >= 0) {
        node->push(w);
        if ((value >> w) & 1) {
            return (!node->child[0] ? 0 : node->child[0]->size) + count_lt(node->child[1], value, w - 1);
        } else {
            return count_lt(node->child[0], value, w - 1);
        }
    } else {
        return 0;
    }
}

//  valueより真に大きい値の個数
template <class T, int W>
int count_gt(NodePtr<T, W> node, T value, int w = W - 1) {
    if (!node) return 0;
    if (w >= 0) {
        node->push(w);
        if ((value >> w) & 1) {
            return count_gt(node->child[1], value, w - 1);
        } else {
            return (!node->child[1] ? 0 : node->child[1]->size) + count_gt(node->child[0], value, w - 1);
        }
    } else {
        return 0;
    }
}

}  //  namespace binary_trie_internal

//  # BinaryTrie
//  binary_trie_internal::Nodeのwrapper.
//  非負整数用の多重集合.
//  kth, min, max, xor_applyなどができる.
//  Wは扱うビット数
template <class T = unsigned, int W = std::numeric_limits<T>::digits>
class BinaryTrie {
  public:
    BinaryTrie(): root_() {}
    ~BinaryTrie() {
        delete root_;
    }
    //  # insert(value)
    //  valueを一個追加する.
    //  O(W)
    void insert(T value) {
        root_ = binary_trie_internal::insert(root_, value);
    }
    //  # erase(value)
    //  valueを一個削除する.
    //  存在しない場合、何もしない
    //  O(W)
    void erase(T value) {
        root_ = binary_trie_internal::erase(root_, value);
    }
    //  # kth(k, value)
    //  各要素に xor value した値を昇順に並べたときの、k番目(0-index)の値を返す
    //  0 <= k < size() でないと壊れる
    //  O(W)
    T kth(int k, T value = 0) {
        assert(!empty());
        assert(0 <= k && k < size());
        return binary_trie_internal::kth(root_, k, value);
    }
    //  # min(value)
    //  min[forall v in S] v xor value
    //  valueを適用した最小値を返す
    //  空だと壊れる
    //  O(W)
    T min(T value = 0) {
        assert(!empty());
        return binary_trie_internal::min(root_, value);
    }
    //  # max(value)
    //  max[forall v in S] v xor value
    //  valueを適用した最大値を返す
    //  空だと壊れる
    //  O(W)
    T max(T value = 0) {
        assert(!empty());
        return binary_trie_internal::max(root_, value);
    }
    //  # size()
    //  要素数(同じ値も個数ぶん数える)
    //  O(1)
    int size() const {
        return root_ ? root_->size : 0;
    }
    //  # contains(value)
    //  valueが含まれるか
    //  O(W)
    bool contains(T value) {
        return binary_trie_internal::count(root_, value) > 0;
    }
    //  # count(value)
    //  valueの個数
    //  O(W)
    int count(T value) {
        return binary_trie_internal::count(root_, value);
    }
    //  # empty()
    //  空か
    bool empty() const {
        return !root_;
    }
    //  # count_lt(value)
    //  value未満の値の個数を返す
    //  O(W)
    int count_lt(T value) {
        return binary_trie_internal::count_lt(root_, value);
    }
    //  # count_gt(value)
    //  valueより大きい値の個数を返す
    //  O(W)
    int count_gt(T value) {
        return binary_trie_internal::count_gt(root_, value);
    }
    //  # apply_xor(value)
    //  全ての要素にxor valueする
    //  O(1)
    void apply_xor(T value) {
        root_ = binary_trie_internal::apply_xor(root_, value);
    }

  private:
    binary_trie_internal::NodePtr<T, W> root_;
};

}  //  namespace nono
