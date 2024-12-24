#pragma once

#include <utility>
#include <vector>

#include "../ds/lazy-segment-tree.hpp"
#include "../ds/segment-tree.hpp"
#include "../graph/base.hpp"
#include "../structure/act-monoid.hpp"
#include "../structure/monoid.hpp"
#include "./tree.hpp"

///  brief : Treeの機能に加えて, パス, 部分木に対する総積を求められる.
///  TODO : lazysegtreeだったり, segtreeだったりがあやふや. いい感じにリファクタリングする.
namespace nono {

//  辺ベースにしたいならEDGE = trueにする
template <class M, class T, bool EDGE = false>
class ProductableTree: public Tree<T> {
    //  using Rev = act_monoid::Rev<M>;
    using Rev = monoid::Rev<M>;

    using Value = M::Value;
    using RevValue = Rev::Value;
    //  using Act = M::Act;

    using Tree<T>::in_;
    using Tree<T>::out_;
    using Tree<T>::head_;
    using Tree<T>::depth_;
    using Tree<T>::parent_;

  public:
    ProductableTree() {}
    ProductableTree(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : ProductableTree(n, edges, std::vector<Value>(n, M::e()), root) {}
    ProductableTree(int n, const std::vector<EdgeBase<T>>& edges, const std::vector<Value>& value, int root = 0)
        : Tree<T>(n, edges, root) {
        std::vector<RevValue> rev_value(n);
        for (int i = 0; i < n; i++) rev_value[in_[i]] = value[i];
        //  segtree_ = LazySegmentTree<Rev>(rev_value);
        segtree_ = SegmentTree<Rev>(rev_value);
    }

    void set(int x, Value value) {
        segtree_.set(in_[x], value);
    }

    void set(int u, int v, Value value) {
        static_assert(EDGE);
        if (depth_[u] < depth_[v]) std::swap(u, v);
        segtree_.set(in_[u], value);
    }

    //  void apply(int x, Act act) {
    //      segtree_.apply(in_[x], act);
    //  }

    //  void apply(int u, int v, Act act) {
    //      static_assert(EDGE);
    //      if (depth_[u] < depth_[v]) std::swap(u, v);
    //      segtree_.apply(in_[u], act);
    //  }

    //  void apply_path(int lhs, int rhs, Act act) {
    //      while (head_[lhs] != head_[rhs]) {
    //          if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
    //          segtree_.apply(in_[head_[rhs]], in_[rhs] + 1, act);
    //          rhs = parent_[head_[rhs]];
    //      }
    //      if (in_[lhs] > in_[rhs]) std::swap(lhs, rhs);
    //      segtree_.apply(in_[lhs] + EDGE, in_[rhs], act);
    //  }

    //  void apply_subtree(int x, Act act) {
    //      segtree_.apply(in_[x] + EDGE, out_[x], act);
    //  }

    //  非可換でもOK
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

    Value prod_subtree(int x) {
        return segtree_.prod(in_[x] + EDGE, out_[x]).ord;
    }

  private:
    //  LazySegmentTree<Rev> segtree_;
    SegmentTree<Rev> segtree_;
};

}  //  namespace nono
