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

///  # copy(root)
//   deep copy
//   O(n)
template <class T>
NodePtr<T> copy(NodePtr<T> root) {
    if (!root) return nullptr;
    push(root);
    NodePtr<T> new_root = new Node<T>(root->element);
    new_root->left = copy(root->left);
    new_root->right = copy(root->right);
    update(new_root);
    return new_root;
}

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
    Sequence(const Sequence& seq): root_(sequence::copy(seq.root_)) {}
    Sequence(Sequence&& seq): root_(std::move(seq.root_)) {
        seq.root_ = nullptr;
    }
    Sequence& operator=(const Sequence& seq) {
        if (root_) delete root_;
        root_ = sequence::copy(seq.root_);
        return *this;
    }
    Sequence& operator=(Sequence&& seq) {
        if (root_) delete root_;
        root_ = std::move(seq.root_);
        seq.root_ = nullptr;
        return *this;
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

    T& operator[](int index) {
        return sequence::kth(root_, index)->element;
    }

    const T& operator[](int index) const {
        return sequence::kth(root_, index)->element;
    }

    void clear() {
        if (root_) delete root_;
        root_ = nullptr;
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

    ///  # split(k)
    ///  return {S[:k], S[k:]}
    ///  S is undefined after split
    ///  O(log n)
    std::pair<Sequence, Sequence> split(int k) {
        auto [lhs, rhs] = sequence::split_by_size(root_, k);
        Sequence first, second;
        first.root_ = lhs;
        second.root_ = rhs;
        root_ = nullptr;
        return {first, second};
    }

    ///  # merge(lhs, rhs)
    ///  return lhs + rhs
    ///  lhs and rhs are undefined after merge
    ///  O(log n)
    friend Sequence merge(Sequence& lhs, Sequence& rhs) {
        Sequence seq;
        seq.root_ = sequence::merge(lhs.root_, rhs.root_);
        lhs.root_ = nullptr;
        rhs.root_ = nullptr;
        return seq;
    }

  private:
    sequence::NodePtr<T> root_ = nullptr;
};

}  //  namespace nono