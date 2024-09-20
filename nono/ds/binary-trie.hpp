#pragma once

#include <cassert>
#include <utility>

namespace nono {

///  brief : 非負整数用のset. kth_elementやxormin/max, 全部にxorなどができる

//  BinaryTrie32とBinaryTrie64が用意してある
template <class T = unsigned, unsigned B = 32>
struct BinaryTrie {
    struct Node;
    using NodePtr = Node*;
    struct Node {
        int count;
        T lazy;
        NodePtr child[2];
        Node(): count(0), lazy(0), child{nullptr, nullptr} {}
        ~Node() {
            if (child[0]) delete child[0];
            if (child[1]) delete child[1];
        }
        void push(int bit) {
            if (lazy == 0) return;
            if ((lazy >> bit) & 1) std::swap(child[0], child[1]);
            if (child[0]) child[0]->lazy ^= lazy;
            if (child[1]) child[1]->lazy ^= lazy;
            lazy = 0;
        }
    };

  public:
    BinaryTrie(): root_(new Node()) {}
    ~BinaryTrie() {
        delete root_;
    }
    void insert(T value) {
        insert(root_, value, 1);
    }
    void erase(T value) {
        insert(root_, value, -1);
    }
    //  0-index
    T kth_element(int k, T value = 0) {
        assert(!empty());
        assert(0 <= k && k < size());
        return kth_element(root_, k, value);
    }
    //  xor valueが最小となるような値 v を返す
    //  ちゃんとxorするのを忘れないこと
    T min_element(T value = 0) {
        assert(!empty());
        return kth_element(root_, 0, value);
    }
    T max_element(T value = 0) {
        assert(!empty());
        return kth_element(root_, size() - 1, value);
    }
    int size() const {
        return root_->count;
    }
    bool contains(T value) {
        if (empty()) return false;
        return min_element(value) == value;
    }
    bool empty() {
        return root_->count == 0;
    }
    int lower_bound(T value) {
        return lower_bound(root_, value);
    }
    int upper_bound(T value) {
        return lower_bound(root_, value + 1);
    }
    //  全ての要素にxor valueする
    void all_xor(T value) {
        root_->lazy ^= value;
    }

  private:
    void insert(NodePtr node, T value, int diff, int bit = B - 1) {
        assert(node);
        node->count += diff;
        if (bit < 0) {
            return;
        }
        node->push(bit);
        auto& next = node->child[(value >> bit) & 1];
        if (!next) next = new Node();
        insert(next, value, diff, bit - 1);
        if (next->count == 0) {
            delete next;
            next = nullptr;
        }
    }

    T kth_element(NodePtr node, int k, T value, int bit = B - 1) {
        assert(node);
        assert(node->count > k);
        if (bit < 0) {
            assert(node->count >= 1);
            return 0;
        }
        node->push(bit);
        T flip = (value >> bit) & 1;
        auto& left = node->child[flip];
        auto& right = node->child[flip ^ 1];
        if (left && left->count > k) {
            return (flip << bit) | kth_element(left, k, value, bit - 1);
        } else {
            if (!right) right = new Node();
            if (left) k -= left->count;
            return ((flip ^ 1) << bit) | kth_element(right, k, value, bit - 1);
        }
    }

    int lower_bound(NodePtr node, T value, int bit = B - 1) {
        assert(node);
        if (bit < 0) {
            return 0;
        }
        node->push(bit);
        if ((value >> bit) & 1) {
            return (node->child[0] ? node->child[0]->count : 0) + lower_bound(node->child[1], value, bit - 1);
        } else {
            return (node->child[0] ? lower_bound(node->child[0], value, bit - 1) : 0);
        }
    }

    NodePtr root_;
};

using BinaryTrie32 = BinaryTrie<unsigned, 32>;
using BinaryTrie64 = BinaryTrie<unsigned long long, 64>;

}  //  namespace nono
