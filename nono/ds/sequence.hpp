#pragma once

#include <cassert>
#include <optional>
#include <utility>

#include "./rbst.hpp"

namespace nono {

namespace sequence_internal {

template <class T>
struct Node {
    Node(T element): element(element) {}
    ~Node() {
        delete left;
        delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;

    T element;
    bool rev = false;
};

template <class M>
using NodePtr = Node<M>*;

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
//  Sのsizeとprodを更新する
//  O(1)
template <class T>
void update(NodePtr<T> root) {
    root->size = 1 + size(root->left) + size(root->right);
}

//  # reverse(root)
//  S <= S[::-1]
//  O(1)
template <class T>
void reverse(NodePtr<T> root) {
    if (!root) return;
    root->rev ^= true;
    std::swap(root->left, root->right);
}

//  # push(root)
//  rev, actを子に伝播する
//  O(1)
template <class T>
void push(NodePtr<T> root) {
    if (root->rev) {
        reverse(root->left);
        reverse(root->right);
        root->rev = false;
    }
}

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

//  # insert(root, index, element)
//  return data[:index] + element + data[index:]
//  O(log n)
template <class T>
NodePtr<T> insert(NodePtr<T> root, int index, T element) {
    assert(0 <= index && index <= size(root));
    auto [lhs, rhs] = split_by_size(root, index);
    return merge(lhs, merge(new Node<T>(element), rhs));
}

//  # erase(root, index)
//  return data[:index] + data[index + 1:]
//  O(log n)
template <class T>
NodePtr<T> erase(NodePtr<T> root, int index) {
    assert(0 <= index && index < size(root));
    auto [lhs, temp] = split_by_size(root, index);
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

//  # kth(root, k)
//  return S[k]
//  存在しなければnullptrを返す
//  O(log n)
template <class T>
NodePtr<T> kth(NodePtr<T> root, int k) {
    return find_by_size(root, k);
}

//  # copy(root)
//  深いコピー
//  O(n)
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

}  //  namespace sequence_internal

//  # Sequence
//  reversable RBST
//  sequence_internal::Nodeのラッパー
//  任意位置挿入、任意位置削除がO(log n)
//  添字が範囲外だと壊れる
template <class T>
class Sequence {
  public:
    Sequence() {}
    ~Sequence() {
        delete root_;
    }
    Sequence(const Sequence& seq): root_(sequence_internal::copy(seq.root_)) {}
    Sequence(Sequence&& seq): root_(std::move(seq.root_)) {
        seq.root_ = nullptr;
    }
    Sequence& operator=(const Sequence& seq) {
        delete root_;
        root_ = sequence_internal::copy(seq.root_);
        return *this;
    }
    Sequence& operator=(Sequence&& seq) {
        delete root_;
        root_ = std::move(seq.root_);
        seq.root_ = nullptr;
        return *this;
    }

    //  # insert(index, element)
    //  S <- S[:index] + [element] + S[index:]
    //  O(log n)
    void insert(int index, T element) {
        root_ = sequence_internal::insert(root_, index, element);
    }

    //  # erase(index)
    //  S <- S[:index] + S[index + 1:]
    //  O(log n)
    void erase(int index) {
        root_ = sequence_internal::erase(root_, index);
    }

    //  # empty()
    //  Sが空か
    //  O(1)
    bool empty() {
        return sequence_internal::empty(root_);
    }

    //  # size()
    //  return |S|
    //  O(1)
    int size() {
        return sequence_internal::size(root_);
    }

    //  # get(index)
    //  return S[index]
    //  O(log n)
    T get(int index) {
        assert(0 <= index && index < size());
        return sequence_internal::find_by_size(root_, index)->element;
    }

    //  # push_back(element)
    //  S <- S + [element]
    //  O(log n)
    void push_back(T element) {
        insert(size(), element);
    }

    //  # pop_back()
    //  S <- S[:size() - 1]
    //  O(log n)
    void pop_back() {
        assert(!empty());
        erase(size() - 1);
    }

    //  # push_front(element)
    //  S <- [element] + S
    //  O(log n)
    void push_front(T element) {
        insert(0, element);
    }

    //  # pop_front()
    //  S <- S[1:]
    //  O(log n)
    void pop_front() {
        assert(!empty());
        erase(0);
    }

    //  # set(index, element)
    //  S[index] <- element
    //  O(log n)
    void set(int index, T element) {
        assert(0 <= index && index < size());
        sequence_internal::find_by_size(root_, index)->element = element;
    }

    //  # operator[](index)
    //  S[index] への参照を返す
    //  O(log n)
    T& operator[](int index) {
        return sequence_internal::kth(root_, index)->element;
    }

    const T& operator[](int index) const {
        return sequence_internal::kth(root_, index)->element;
    }

    //  # clear()
    //  S <- []
    //  O(n)
    void clear() {
        delete root_;
        root_ = nullptr;
    }

    //  # reverse(left, right)
    //  S <- S[:left] + S[left:right:-1] + S[right:]
    //  O(log n)
    void reverse(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = sequence_internal::split_by_size(root_, left);
        auto [mhs, rhs] = sequence_internal::split_by_size(temp, right - left);
        sequence_internal::reverse(mhs);
        root_ = sequence_internal::merge(sequence_internal::merge(lhs, mhs), rhs);
    }

    //  # split(k)
    //  return {S[:k], S[k:]}
    //  split後のSは空になる
    //  O(n)。戻り値を作るときに両方を丸ごとコピーするので、O(log n) にならない
    std::pair<Sequence, Sequence> split(int k) {
        auto [lhs, rhs] = sequence_internal::split_by_size(root_, k);
        Sequence first, second;
        first.root_ = lhs;
        second.root_ = rhs;
        root_ = nullptr;
        return {first, second};
    }

    //  # merge(lhs, rhs)
    //  return lhs + rhs
    //  merge後のlhs, rhsは空になる
    //  O(log n)
    friend Sequence merge(Sequence& lhs, Sequence& rhs) {
        Sequence seq;
        seq.root_ = sequence_internal::merge(lhs.root_, rhs.root_);
        lhs.root_ = nullptr;
        rhs.root_ = nullptr;
        return seq;
    }

  private:
    sequence_internal::NodePtr<T> root_ = nullptr;
};

}  //  namespace nono
