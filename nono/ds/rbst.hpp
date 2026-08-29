#pragma once

#include <random>
#include <utility>

namespace nono {

namespace rbst {

inline std::mt19937 rng(std::random_device{}());

///  NodePtrは->left, ->right, ->size(int)を持つこと
///  push(NodePtr)/update(NodePtr)はNode型ごとに呼び出し側の名前空間でADLにより解決される

///  # size(root)
///  O(1)
template <class NodePtr>
int size(NodePtr root) {
    return root ? root->size : 0;
}

///  # merge(lhs, rhs)
///  return root of (lhs + rhs)
///  O(log n)
template <class NodePtr>
NodePtr merge(NodePtr lhs, NodePtr rhs) {
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
template <class NodePtr>
std::pair<NodePtr, NodePtr> split_by_size(NodePtr root, int k) {
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

///  # find by size(root, k)
///  return S[k]
///  if not exist, return nullptr
///  O(log n)
template <class NodePtr>
NodePtr find_by_size(NodePtr root, int k) {
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

}  //  namespace rbst

}  //  namespace nono
