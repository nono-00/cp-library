#pragma once

#include <cassert>
#include <optional>
#include <utility>

#include "../structure/act-monoid/rev.hpp"
#include "./rbst.hpp"

namespace nono {

namespace dynamic_sequence_internal {

template <class M>
struct Node {
    Node(M::Value element): element(element), total(element), act(std::nullopt) {}
    ~Node() {
        delete left;
        delete right;
    }
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 1;

    act_monoid::Rev<M>::Value element, total;
    std::optional<typename M::Act> act;
    bool rev = false;
};

template <class M>
using NodePtr = Node<M>*;

//  # empty(root)
//  Sが空か
//  O(1)
template <class M>
bool empty(NodePtr<M> root) {
    return !root;
}

//  # size(root)
//  |S|
//  O(1)
template <class M>
int size(NodePtr<M> root) {
    return root ? root->size : 0;
}

//  # prod(root)
//  return op[for v in S](v)
//  O(1)
template <class M>
act_monoid::Rev<M>::Value prod(NodePtr<M> root) {
    return root ? root->total : M::e();
}

//  # update(root)
//  Sのsizeとprodを更新する
//  O(1)
template <class M>
void update(NodePtr<M> root) {
    using Rev = act_monoid::Rev<M>;
    root->size = 1 + size(root->left) + size(root->right);
    root->total = Rev::op(Rev::op(prod(root->left), root->element), prod(root->right));
}

//  # apply(root, act)
//  [for v in S](v <= mapping(act, v))
//  O(1)
template <class M>
void apply(NodePtr<M> root, typename M::Act act) {
    using Rev = act_monoid::Rev<M>;
    if (!root) return;
    root->element = Rev::mapping(act, root->element);
    root->total = Rev::mapping(act, root->total);
    root->act = Rev::composition(act, root->act ? *root->act : M::id());
}

//  # reverse(root)
//  S <= S[::-1]
//  O(1)
template <class M>
void reverse(NodePtr<M> root) {
    if (!root) return;
    root->rev ^= true;
    std::swap(root->total.ord, root->total.rev);
    std::swap(root->left, root->right);
}

//  # push(root)
//  rev, actを子に伝播する
//  O(1)
template <class M>
void push(NodePtr<M> root) {
    if (root->rev) {
        reverse(root->left);
        reverse(root->right);
        root->rev = false;
    }
    if (root->act) {
        apply(root->left, *root->act);
        apply(root->right, *root->act);
        root->act = std::nullopt;
    }
}

//  # merge(lhs, rhs)
//  lhs + rhs の根を返す
//  O(log n)
template <class M>
NodePtr<M> merge(NodePtr<M> lhs, NodePtr<M> rhs) {
    return rbst_internal::merge(lhs, rhs);
}

//  # split_by_size(root, k)
//  (S[:k] の根, S[k:] の根) を返す
//  size(S) < k なら (S, 空) を返す
//  O(log n)
template <class M>
std::pair<NodePtr<M>, NodePtr<M>> split_by_size(NodePtr<M> root, int k) {
    return rbst_internal::split_by_size(root, k);
}

//  # insert(root, index, element)
//  return data[:index] + element + data[index:]
//  O(log n)
template <class M>
NodePtr<M> insert(NodePtr<M> root, int index, typename M::Value element) {
    assert(0 <= index && index <= size(root));
    auto [lhs, rhs] = split_by_size(root, index);
    return merge(lhs, merge(new Node<M>(element), rhs));
}

//  # erase(root, index)
//  return data[:index] + data[index + 1:]
//  O(log n)
template <class M>
NodePtr<M> erase(NodePtr<M> root, int index) {
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
template <class M>
NodePtr<M> find_by_size(NodePtr<M> root, int k) {
    return rbst_internal::find_by_size(root, k);
}

//  # kth(root, k)
//  return S[k]
//  存在しなければnullptrを返す
//  O(log n)
template <class M>
NodePtr<M> kth(NodePtr<M> root, int k) {
    return find_by_size(root, k);
}

}  //  namespace dynamic_sequence_internal

//  # DynamicSequence
//  reversable RBST
//  dynamic_sequence_internal::Nodeのラッパー
//  区間反転、区間作用、区間積, 任意位置挿入、任意位置削除がO(log n)
//  Mはact-monoid。添字が範囲外だと壊れる
template <class M>
class DynamicSequence {
    using T = M::Value;
    using F = M::Act;

  public:
    DynamicSequence() {}
    ~DynamicSequence() {
        delete root_;
    }

    //  # insert(index, element)
    //  S <= S[:index] + [element] + S[index:]
    //  O(log n)
    void insert(int index, T element) {
        root_ = dynamic_sequence_internal::insert(root_, index, element);
    }

    //  # erase(index)
    //  S <= S[:index] + S[index + 1:]
    //  O(log n)
    void erase(int index) {
        root_ = dynamic_sequence_internal::erase(root_, index);
    }

    //  # empty()
    //  Sが空か
    //  O(1)
    bool empty() {
        return dynamic_sequence_internal::empty(root_);
    }

    //  # size()
    //  return |S|
    //  O(1)
    int size() {
        return dynamic_sequence_internal::size(root_);
    }

    //  # get(index)
    //  return S[index]
    //  O(log n)
    T get(int index) {
        assert(0 <= index && index < size());
        return dynamic_sequence_internal::find_by_size(root_, index)->element.ord;
    }

    //  # push_back(element)
    //  S <= S + [element]
    //  O(log n)
    void push_back(T element) {
        insert(size(), element);
    }

    //  # pop_back()
    //  S <= S[:size() - 1]
    //  O(log n)
    void pop_back() {
        assert(!empty());
        erase(size() - 1);
    }

    //  # push_front(element)
    //  S <= [element] + S
    //  O(log n)
    void push_front(T element) {
        insert(0, element);
    }

    //  # pop_front()
    //  S <= S[1:]
    //  O(log n)
    void pop_front() {
        assert(!empty());
        erase(0);
    }

    //  # set(index, element)
    //  S[index] <= element
    //  O(log n)
    void set(int index, T element) {
        assert(0 <= index && index < size());
        erase(index);
        insert(index, element);
    }

    //  # reverse(left, right)
    //  S <= S[:left] + S[left:right:-1] + S[right:]
    //  O(log n)
    void reverse(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_internal::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_internal::split_by_size(temp, right - left);
        dynamic_sequence_internal::reverse(mhs);
        root_ = dynamic_sequence_internal::merge(dynamic_sequence_internal::merge(lhs, mhs), rhs);
    }

    //  # apply(left, right, act)
    //  [for i in [left, right)](S[i] <= mapping(act, S[i]))
    //  O(log n)
    void apply(int left, int right, F act) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_internal::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_internal::split_by_size(temp, right - left);
        dynamic_sequence_internal::apply(mhs, act);
        root_ = dynamic_sequence_internal::merge(dynamic_sequence_internal::merge(lhs, mhs), rhs);
    }

    //  # prod(left, right)
    //  return op[for i in [left, right)](S[i])
    //  O(log n)
    T prod(int left, int right) {
        assert(0 <= left && left <= right && right <= size());
        auto [lhs, temp] = dynamic_sequence_internal::split_by_size(root_, left);
        auto [mhs, rhs] = dynamic_sequence_internal::split_by_size(temp, right - left);
        auto ans = dynamic_sequence_internal::prod(mhs);
        root_ = dynamic_sequence_internal::merge(dynamic_sequence_internal::merge(lhs, mhs), rhs);
        return ans.ord;
    }

  private:
    dynamic_sequence_internal::NodePtr<M> root_ = nullptr;
};

}  //  namespace nono
