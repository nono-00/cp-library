#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <optional>

namespace nono {

///  # 動的LiChao木
///  直線, 線分追加, ある地点での最小値/最大値取得ができる
///  MinDynamicLiChaoTree, MaxDynamicLiChaoTreeを定義してあるので使うと良い
template <class T, class Index = int, bool minimum = true>
class DynamicLiChaoTree {
    static constexpr T compare = minimum ? 1 : -1;
    struct Line {
        Line(T a = 0, T b = 0): a(a), b(b) {}
        T a, b;
        T eval(Index x) {
            return a * x + b;
        }
    };
    struct Node;
    using NodePtr = Node*;
    struct Node {
        NodePtr l = nullptr;
        NodePtr r = nullptr;
        std::optional<Line> v;
        Node(): v(std::nullopt) {}
        Node(Line v): v(v) {}
        ~Node() {
            if (l) delete l;
            if (r) delete r;
        }
    };

  public:
    ///  # DynamicLiChaoTree(lb, ub)
    ///  [lb, ub)の領域を持つLiChao木を作る
    DynamicLiChaoTree(Index lb = std::numeric_limits<Index>::min(), Index ub = std::numeric_limits<Index>::max())
        : lb_(lb),
          ub_(ub),
          root_(new Node()) {}
    ~DynamicLiChaoTree() {
        delete root_;
    }
    ///  # add_line(a, b)
    ///  [lb, ub)にax + bを追加する
    ///  O(log(ub - lb))
    void add_line(T a, T b) {
        add_segment(lb_, ub_, a, b);
    }
    ///  # add_segment
    ///  [l, r)にax+bを追加
    ///  O(log(ub - lb))
    void add_segment(Index l, Index r, T a, T b) {
        assert(lb_ <= l && l <= r && r <= ub_);
        add(root_, lb_, ub_, l, r, Line(compare * a, compare * b));
    }
    ///  # get(x)
    ///  min/max[forall (a, b) in {Line or Segment}](ax + b)
    ///  線分、直線が存在しない場合、nullopt
    std::optional<T> get(Index x) {
        assert(lb_ <= x && x < ub_);
        auto result = get(root_, lb_, ub_, x);
        return result ? compare * *result : result;
    }

  private:
    void add(NodePtr p, Index node_lb, Index node_ub, Index lb, Index ub, Line v) {
        assert(p);
        if (node_ub <= lb || ub <= node_lb) return;
        if (lb <= node_lb && node_ub <= ub) {
            update(p, node_lb, node_ub, v);
        } else {
            Index m = std::midpoint(node_lb, node_ub);
            if (!p->l) p->l = new Node();
            if (!p->r) p->r = new Node();
            add(p->l, node_lb, m, lb, ub, v);
            add(p->r, m, node_ub, lb, ub, v);
        }
    }
    void update(NodePtr p, Index node_lb, Index node_ub, Line v) {
        assert(p);
        if (!p->v) {
            p->v = v;
            return;
        }
        Index m = std::midpoint(node_lb, node_ub);
        bool lhs = p->v->eval(node_lb) < v.eval(node_lb);
        bool mhs = p->v->eval(m) < v.eval(m);
        bool rhs = p->v->eval(node_ub - 1) < v.eval(node_ub - 1);
        if (lhs && mhs && rhs) return;
        if (!lhs && !mhs && !rhs) {
            p->v = v;
            return;
        }
        if (lhs ^ mhs ^ rhs) {
            std::swap(v, *(p->v));
            lhs ^= true, mhs ^= true, rhs ^= true;
        }
        if (lhs) {
            if (!p->r) p->r = new Node();
            update(p->r, m, node_ub, v);
        } else {
            if (!p->l) p->l = new Node();
            update(p->l, node_lb, m, v);
        }
    }
    std::optional<T> get(NodePtr p, Index node_lb, Index node_ub, Index i) {
        if (!p) return std::nullopt;
        Index m = std::midpoint(node_lb, node_ub);
        std::optional<T> v = p->v ? std::optional<T>(p->v->eval(i)) : std::nullopt;
        if (node_lb <= i && i < m) {
            auto result = get(p->l, node_lb, m, i);
            if (v && result) {
                v = std::min(v, result);
            } else if (result) {
                v = result;
            }
        } else if (m <= i && i < node_ub) {
            auto result = get(p->r, m, node_ub, i);
            if (v && result) {
                v = std::min(v, result);
            } else if (result) {
                v = result;
            }
        }
        return v;
    }

  private:
    Index lb_;
    Index ub_;
    NodePtr root_;
};

template <class T, class Index = int>
using MinDynamicLiChaoTree = DynamicLiChaoTree<T, Index, true>;

template <class T, class Index = int>
using MaxDynamicLiChaoTree = DynamicLiChaoTree<T, Index, false>;

}  //  namespace nono
