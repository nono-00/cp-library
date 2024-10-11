#pragma once

#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/rooted-tree.hpp"

namespace nono {

//  real vertex := 元の木に存在する頂点. 値を持っていたりいなかったり.
//
//  virtual vertex := 元の木に存在する頂点から値の情報を抜いた仮想頂点.
//                    たくさん複製される.
//
//  cluster := 分解の過程でできる部分木みたいな何か. 部分木とは言い難いものができるので代わりの用語として使用.
//
//  path cluster := real vertex を根に持つ cluster を heavy edge でくっつけた cluster.
//  point cluster := 同一の virtual vertex を根とする cluster をくっつけた cluster.

//  分解過程での操作
//
//  [1] 根に繋がる heavy edge が存在する場合
//      path cluster なので 分解する
//
//  [2] 根に繋がる heavy edge が存在せず, real cluster の場合
//      根の real vertex を取り除いて virtual vertex を生やし point cluster にする
//
//  [3] light edge が複数個繋がっている場合
//      point cluster なので virtual vertex を複製, 分解する
//
//  [4] light edge が一つの場合
//      virtual vertex, light edge を取り除く
//
//
//  対応する逆操作
//  [1] compress
//      隣接する path cluster を heavy edge でくっつける
//
//  [2] add_vertex
//      根の virtual vertex を取り除いて, 対応する real vertex を生やす
//
//  [3] rake
//      同一の virtual vertex を根に持つ cluster をくっつける
//
//  [4] add_edge
//      light edge をくっつけて もう一つの端点に virtual vertex を生やす
//
//  path cluster は heavy edge の 集約値と現状のcluster の値を持つ
//  point cluster は cluster の値を持つ
//
//  path cluster の合成 compress は
//  [1] heavy edge の集約値 の合成
//  [2] (根に近い方の path cluster の集約値) +
//      ({根に近い方の path cluster のheavy edge の集約値} と {根から遠い方の path cluster の集約値})
//  で構成される
//
//  point cluster の合成 rake は
//  [1] clueter の集約値 の 演算
//  で構成される
//
//  各頂点には対応する値が入っている

//  compress(u)
//      サイズ 1 だった場合は対応する cluster を返す.
//      heavy edge path 上の頂点に対して rake して (rake vertex, size) 配列を管理する
//      これらを merge する.
//
//  rake(u)
//      graph[u][1:] が空の場合は 単位 cluster を返す.
//      graph[u][1:] の各頂点に対して compress して (compress vertex, size) 配列を管理する
//      これらを merge する.
//
//
template <class M>
class StaticTopTree {
    using Value = M::Value;
    using Path = M::Act;

    struct Cluster {
        Cluster(Value value, Path path, bool reset = false): value(value), path(path), reset(reset) {}
        Value value;
        Path path;
        bool reset = false;
    };

    //  rhs <- lhs <- ... <- root
    //  lhs が上
    static Cluster op(Cluster lhs, Cluster rhs) {
        return Cluster{M::op(lhs.value, M::mapping(lhs.path, rhs.value)),
                       lhs.reset ? lhs.path : M::composition(lhs.path, rhs.path)};
    }

  public:
    StaticTopTree() {}
    StaticTopTree(int n, const std::vector<EdgeBase<Path>>& edges, int root = 0)
        : StaticTopTree(n, edges, std::vector<Value>(n, M::e()), root) {}
    StaticTopTree(int n, const std::vector<EdgeBase<Path>>& edges, const std::vector<Value>& values, int root = 0)
        : n_(n),
          nodes_(n + 1, {M::e(), M::id(), false}),
          left_(n + 1, -1),
          right_(n + 1, -1),
          parent_(n + 1, -1),
          to_(n - 1, -1),
          graph_(to_rooted_tree(n, edges, root)),
          values_(values) {
        for (int i = 0; i < n_; i++) nodes_[i].value = values_[i];
        nodes_[n_].reset = true;
        dfs(root);
        root_ = compress(root).first;
    }

    Value all_prod() {
        return nodes_[root_].value;
    }

    void set_vertex(int u, Value value) {
        nodes_[u].value = M::mapping(nodes_[u].path, values_[u] = value);
        update(u);
    }

    void set_edge(int i, Path edge) {
        nodes_[to_[i]].path = edge;
        nodes_[to_[i]].value = M::mapping(edge, values_[to_[i]]);
        update(to_[i]);
    }

    //  (sst vertex, size)
    std::pair<int, int> compress(int u) {
        std::vector<std::pair<int, int>> seq;
        while (true) {  //  葉では無い間rakeし続ける
            seq.emplace_back(rake(u));
            if (graph_[u].empty()) break;
            u = graph_[u][0].to;
        }
        return add({n_, 0}, merge(seq));
    }

    std::pair<int, int> rake(int u) {
        if (graph_[u].size() <= 1) return {u, 1};
        std::vector<std::pair<int, int>> seq;
        seq.push_back({u, 1});
        for (int i = 0; auto e: graph_[u]) {
            if (i != 0) seq.emplace_back(compress(e.to));
            i++;
        }
        return merge(seq);
    }

    std::pair<int, int> add(std::pair<int, int> lhs, std::pair<int, int> rhs) {
        int id = nodes_.size();
        nodes_.emplace_back(op(nodes_[lhs.first], nodes_[rhs.first]));
        parent_.push_back(-1);
        left_.push_back(-1);
        right_.push_back(-1);
        parent_[lhs.first] = parent_[rhs.first] = id;
        left_[id] = lhs.first;
        right_[id] = rhs.first;
        return {id, lhs.second + rhs.second};
    }

    std::pair<int, int> merge(const std::vector<std::pair<int, int>>& seq) {
        if (seq.size() == 1) return seq.front();
        std::vector<std::pair<int, int>> lseq, rseq;
        int u = 0;
        for (auto& [_, s]: seq) u += s;
        for (auto& [i, s]: seq) (u > s ? lseq : rseq).emplace_back(i, s), u -= s * 2;
        return add(merge(lseq), merge(rseq));
    }

  private:
    void update(int i) {
        while (i != root_) {
            i = parent_[i];
            nodes_[i] = op(nodes_[left_[i]], nodes_[right_[i]]);
        }
    }

    int dfs(int u) {
        int size_sum = 1;
        int max_size = 0;
        int max_size_i = 0;
        auto&& adj = graph_[u];
        for (int i = 0; i < std::ssize(adj); i++) {
            const auto& e = adj[i];
            nodes_[e.to].path = e.weight;
            nodes_[e.to].value = M::mapping(e.weight, nodes_[e.to].value);
            to_[e.id] = e.to;
            int size = dfs(e.to);
            size_sum += size;
            if (max_size < size) {
                max_size = size;
                max_size_i = i;
            }
        }
        if (max_size_i != 0) {
            std::swap(adj[0], adj[max_size_i]);
        }
        return size_sum;
    }

    int n_;
    int root_;
    std::vector<Cluster> nodes_;
    std::vector<int> left_;
    std::vector<int> right_;
    std::vector<int> parent_;
    std::vector<int> to_;
    Graph<Path> graph_;
    std::vector<Value> values_;
};

}  //  namespace nono
