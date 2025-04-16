#pragma once

#include <cassert>
#include <optional>
#include <random>
#include <utility>

namespace nono {

namespace sequence {

template <class T>
struct Node {
    Node(T element): element(element) {}
    ~Node() {
        if (left) delete left;
        if (right) delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;

    T element;
    bool rev = false;
};

template <class M>
using NodePtr = Node<M>*;

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
///  update size of S, prod of S
///  O(1)
template <class T>
void update(NodePtr<T> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

///  # reverse(root)
///  S <= S[::-1]
///  O(1)
template <class T>
void reverse(NodePtr<T> root) {
    if (!root) return;
    root->rev ^= true;
    std::swap(root->left, root->right);
}

///  # push(root)
///  propagate rev, act
///  O(1)
template <class T>
void push(NodePtr<T> root) {
    if (root->rev) {
        reverse(root->left);
        reverse(root->right);
        root->rev = false;
    }
}

///  # merge(lhs, rhs)
///  return root of (lhs + rhs)
///  O(log n)
template <class T>
NodePtr<T> merge(NodePtr<T> lhs, NodePtr<T> rhs) {
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
template <class T>
std::pair<NodePtr<T>, NodePtr<T>> split_by_size(NodePtr<T> root, int k) {
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
template <class T>
NodePtr<T> insert(NodePtr<T> root, int index, T element) {
    assert(0 <= index && index <= size(root));
    auto [lhs, rhs] = split_by_size(root, index);
    return merge(lhs, merge(new Node<T>(element), rhs));
}

///  # erase(root, index)
///  return data[:index] + data[index + 1:]
///  O(log n)
template <class T>
NodePtr<T> erase(NodePtr<T> root, int index) {
    assert(0 <= index && index < size(root));
    auto [lhs, temp] = split_by_size(root, index);
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
template <class T>
NodePtr<T> kth(NodePtr<T> root, int k) {
    return find_by_size(root, k);
}

template <class T>
class Iterator {
  public:
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::bidirectional_iterator_tag;

    Iterator(): current(nullptr) {}
    Iterator(NodePtr<T> node): current(node) {}

    reference operator*() const {
        return current->value;
    }
    pointer operator->() const {
        return &current->value;
    }

    Iterator& operator++() {
        if (current) {
            if (current->right) {
                current = current->right;
                while (current->left) current = current->left;
            } else {
                NodePtr<T> parent = current->parent;
                while (parent && current == parent->right) {
                    current = parent;
                    parent = current->parent;
                }
                current = parent;
            }
        }
        return *this;
    }

    Iterator operator++(int) {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    Iterator& operator--() {
        if (current) {
            if (current->left) {
                current = current->left;
                while (current->right) current = current->right;
            } else {
                NodePtr<T> parent = current->parent;
                while (parent && current == parent->left) {
                    current = parent;
                    parent = current->parent;
                }
                current = parent;
            }
        }
        return *this;
    }

    Iterator operator--(int) {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return lhs.current == rhs.current;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return !(lhs == rhs);
    }

  private:
    NodePtr<T> current;
};

static_assert(std::bidirectional_iterator<Iterator<int>>);

}  //  namespace sequence

///  # Sequence
///  reversable RBST
///  sequence::Nodeのラッパー
///  任意位置挿入、任意位置削除がO(log n)
template <class T>
class Sequence {
  public:
    Sequence() {}
    ~Sequence() {
        if (root_) delete root_;
    }

    ///  # insert(index, element)
    ///  S <- S[:index] + [element] + S[index:]
    ///  O(log n)
    void insert(int index, T element) {
        root_ = sequence::insert(root_, index, element);
    }

    ///  # erase(index)
    ///  S <- S[:index] + S[index + 1:]
    ///  O(log n)
    void erase(int index) {
        root_ = sequence::erase(root_, index);
    }

    ///  # empty()
    ///  whether S is empty
    ///  O(1)
    bool empty() {
        return sequence::empty(root_);
    }

    ///  # size()
    ///  return |S|
    ///  O(1)
    int size() {
        return sequence::size(root_);
    }

    ///  # get(index)
    ///  return S[index]
    ///  O(log n)
    T get(int index) {
        assert(0 <= index && index < size());
        return sequence::find_by_size(root_, index)->element;
    }

    ///  # push_back(element)
    ///  S <- S + [element]
    ///  O(log n)
    void push_back(T element) {
        insert(size(), element);
    }

    ///  # pop_back()
    ///  S <- S[:size() - 1]
    ///  O(log n)
    void pop_back() {
        assert(!empty());
        erase(size() - 1);
    }

    ///  # push_front(element)
    ///  S <- [element] + S
    ///  O(log n)
    void push_front(T element) {
        insert(0, element);
    }

    ///  # pop_front()
    ///  S <- S[1:]
    ///  O(log n)
    void pop_front() {
        assert(!empty());
        erase(0);
    }

    ///  # set(index, element)
    ///  S[index] <- element
    ///  O(log n)
    void set(int index, T element) {
        assert(0 <= index && index < size());
        sequence::find_by_size(root_, index)->element = element;
    }

    ///  # reverse(left, right)
    ///  S <- S[:left] + S[left:right:-1] + S[right:]
    ///  O(log n)
    void reverse(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = sequence::split_by_size(root_, left);
        auto [mhs, rhs] = sequence::split_by_size(temp, right - left);
        sequence::reverse(mhs);
        root_ = sequence::merge(sequence::merge(lhs, mhs), rhs);
    }

  private:
    sequence::NodePtr<T> root_ = nullptr;
};

}  //  namespace nono