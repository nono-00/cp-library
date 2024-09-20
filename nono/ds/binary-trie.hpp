#pragma once

#include <cassert>
#include <numeric>
#include <vector>

namespace nono {

template <class T, int N = 15000000>
class MemoryPool {
  public:
    MemoryPool(): head_(N) {
        std::iota(std::begin(stack_), std::end(stack_), 0);
    }

    T* allocate() {
        return pool_ + stack_[--head_];
    }

    void deallocate(T* ptr) {
        if (ptr) stack_[head_++] = ptr - pool_;
    }

  private:
    T pool_[N];
    int stack_[N];
    int head_;
};

template <class T = unsigned, unsigned B = 32>
struct BinaryTrie {
    struct Node;
    using NodePtr = Node*;
    struct Node {
        int count;
        T lazy;
        NodePtr child[2];
        Node(): count(0), lazy(0), child{nullptr, nullptr} {}
        static void* operator new(std::size_t) {
            return pool_.allocate();
        }
        static void operator delete(void* ptr) {
            pool_.deallocate(static_cast<NodePtr>(ptr));
        }
        void push(int bit) {
            if (lazy == 0) return;
            if ((lazy >> bit) & 1) std::swap(child[0], child[1]);
            if (child[0]) child[0]->lazy ^= lazy;
            if (child[1]) child[1]->lazy ^= lazy;
            lazy = 0;
        }
    };
    static MemoryPool<Node> pool_;

  public:
    BinaryTrie(): root_(new Node()) {}
    void insert(T value) {
        insert(root_, value, 1);
    }
    void erase(T value) {
        insert(root_, value, -1);
    }
    T kth_element(int k, T value = 0) {
        assert(0 <= k && k < size());
        return kth_element(root_, k, value);
    }
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

template <class T, unsigned B>
MemoryPool<typename BinaryTrie<T, B>::Node> BinaryTrie<T, B>::pool_;

}  //  namespace nono
