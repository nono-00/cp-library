#pragma once

#include <utility>
#include <vector>

#include "../ds/lazy-segment-tree.hpp"
#include "../graph/base.hpp"
#include "../structure/act-monoid/rev.hpp"
#include "./tree.hpp"

namespace nono {

//  # ProductableTree
//  Treeに、パスと部分木に対する総積(prod)と作用(apply)を加えたもの
//  Mはact-monoid。EDGE = false なら頂点に、EDGE = true なら辺に値を持つ
//  辺に持たせる場合、値はコンストラクタで初期化できないので set(u, v, value) で設定する
template <class M, class T, bool EDGE = false>
class ProductableTree: public Tree<T> {
    using Rev = act_monoid::Rev<M>;

    using Value = M::Value;
    using RevValue = Rev::Value;
    using Act = M::Act;

    using Tree<T>::in_;
    using Tree<T>::out_;
    using Tree<T>::head_;
    using Tree<T>::depth_;
    using Tree<T>::parent_;

  public:
    ProductableTree() {}

    //  # ProductableTree(n, edges, root)
    //  全ての値を M::e() で初期化する
    //  edgesが木でないと壊れる
    //  O(n)
    ProductableTree(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : ProductableTree(n, edges, std::vector<Value>(n, M::e()), root) {}

    //  # ProductableTree(n, edges, value, root)
    //  頂点iの値を value[i] で初期化する
    //  edgesが木でないと壊れる
    //  O(n)
    ProductableTree(int n, const std::vector<EdgeBase<T>>& edges, const std::vector<Value>& value, int root = 0)
        : Tree<T>(n, edges, root) {
        std::vector<RevValue> rev_value(n);
        for (int i = 0; i < n; i++) rev_value[in_[i]] = value[i];
        segtree_ = LazySegmentTree<Rev>(rev_value);
    }

    //  # set(x, value)
    //  頂点xの値 <- value
    //  O(log n)
    void set(int x, Value value) {
        segtree_.set(in_[x], value);
    }

    //  # set(u, v, value)
    //  辺(u, v)の値 <- value。EDGE = true のときだけ使える
    //  O(log n)
    void set(int u, int v, Value value) {
        static_assert(EDGE);
        if (depth_[u] < depth_[v]) std::swap(u, v);
        segtree_.set(in_[u], value);
    }

    //  # apply(x, act)
    //  頂点xの値 <- mapping(act, 頂点xの値)
    //  O(log n)
    void apply(int x, Act act) {
        segtree_.apply(in_[x], act);
    }

    //  # apply(u, v, act)
    //  辺(u, v)の値 <- mapping(act, 辺(u, v)の値)。EDGE = true のときだけ使える
    //  O(log n)
    void apply(int u, int v, Act act) {
        static_assert(EDGE);
        if (depth_[u] < depth_[v]) std::swap(u, v);
        segtree_.apply(in_[u], act);
    }

    //  # apply_path(lhs, rhs, act)
    //  lhsからrhsへのパス上の値すべてにactを作用させる
    //  [for w in path(lhs, rhs)](w_weight <- mapping(act, w_weight))
    //  不具合があり、正しく動かない。最後に処理する区間で、深いほうの端の値に作用が届かない。
    //  EDGE = true では、区間が逆転して壊れることもある
    //  O((log n)^2)
    void apply_path(int lhs, int rhs, Act act) {
        while (head_[lhs] != head_[rhs]) {
            if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
            segtree_.apply(in_[head_[rhs]], in_[rhs] + 1, act);
            rhs = parent_[head_[rhs]];
        }
        if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
        segtree_.apply(in_[lhs] + EDGE, in_[rhs], act);
    }

    //  # apply_subtree(x, act)
    //  xの部分木の値すべてにactを作用させる
    //  [for w in subtree(x)](w_weight <- mapping(act, w_weight))
    //  O(log n)
    void apply_subtree(int x, Act act) {
        segtree_.apply(in_[x] + EDGE, out_[x], act);
    }

    //  # prod_path(from, to)
    //  fromからtoへのパス上の値を、fromからtoへ向かう順に並べた総積を返す
    //  O((log n)^2)
    Value prod_path(int from, int to) {
        Value sml = M::e();
        Value smr = M::e();
        while (head_[from] != head_[to]) {
            if (in_[from] > in_[to]) {
                sml = M::op(sml, segtree_.prod(in_[head_[from]], in_[from] + 1).rev);
                from = parent_[head_[from]];
            } else {
                smr = M::op(segtree_.prod(in_[head_[to]], in_[to] + 1).ord, smr);
                to = parent_[head_[to]];
            }
        }
        if (in_[from] < in_[to]) {
            smr = M::op(segtree_.prod(in_[from] + EDGE, in_[to] + 1).ord, smr);
        } else {
            sml = M::op(sml, segtree_.prod(in_[to] + EDGE, in_[from] + 1).rev);
        }
        return M::op(sml, smr);
    }

    //  # prod_subtree(x)
    //  xの部分木の値を、行きがけ順に並べた総積を返す
    //  O(log n)
    Value prod_subtree(int x) {
        return segtree_.prod(in_[x] + EDGE, out_[x]).ord;
    }

  private:
    LazySegmentTree<Rev> segtree_;
};

}  //  namespace nono
