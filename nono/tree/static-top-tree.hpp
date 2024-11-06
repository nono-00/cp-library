#pragma once

#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/rooted-tree.hpp"

///  brief : static top tree. to_static_top_tree で作れる. 
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

namespace static_top_tree {

enum class NodeType {
    Vertex,
    AddVertex,
    AddEdge,
    Compress,
    Rake
};

struct Node {
    NodeType type;
    int left;
    int right;
    int parent;
};

}  //  namespace static_top_tree

struct StaticTopTree {
    int root;
    std::vector<static_top_tree::Node> nodes;
    std::vector<int> vertexs;
    std::vector<int> edges;
};

namespace internal {

template <class T>
class StaticTopTreeBuilder {
    using Node = static_top_tree::Node;
    using NodeType = static_top_tree::NodeType;

  public:
    StaticTopTreeBuilder(int n, const std::vector<EdgeBase<T>>& edges, int root = 0)
        : n_(n),
          graph_(to_rooted_tree(n, edges, root)),
          vertexs_(n),
          edges_(n - 1) {
        assert(is_tree(to_undirected_graph(n, edges)));
        dfs(root);
        root_ = compress(root).first;
    }

    StaticTopTree static_top_tree() {
        return StaticTopTree{root_, nodes_, vertexs_, edges_};
    }

  private:
    int dfs(int u) {
        int size_sum = 1;
        int max_size = 0;
        int max_size_i = 0;
        auto&& adj = graph_[u];
        for (int i = 0; i < std::ssize(adj); i++) {
            const auto& e = adj[i];
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

    //  (sst vertex, size)
    std::pair<int, int> compress_merge(const std::vector<std::pair<int, std::pair<int, int>>>& seq) {
        if (seq.size() == 1) return seq.front().second;
        std::vector<std::pair<int, std::pair<int, int>>> lseq, rseq;
        int u = 0;
        for (auto& [_, p]: seq) u += p.second;
        for (auto& [i, p]: seq) (u > p.second ? lseq : rseq).emplace_back(i, p), u -= p.second * 2;
        auto lhs = compress_merge(lseq);
        auto rhs = compress_merge(rseq);
        auto result = add(lhs.first, rhs.first, NodeType::Compress);
        edges_[rseq.front().first] = result;
        return {result, lhs.second + rhs.second};
    }

    std::pair<int, int> compress(int u) {
        std::vector<std::pair<int, std::pair<int, int>>> seq;
        int i = -1;
        while (true) {  //  葉では無い間rakeし続ける
            seq.emplace_back(i, add_vertex(u));
            if (graph_[u].empty()) break;
            i = graph_[u][0].id;
            u = graph_[u][0].to;
        }
        return compress_merge(seq);
    }

    std::pair<int, int> add_vertex(int u) {
        if (graph_[u].size() <= 1) return vertex(u);
        auto light = rake(u);
        auto result = add(light.first, -1, NodeType::AddVertex);
        vertexs_[u] = result;
        return {result, 1 + light.second};
    }

    std::pair<int, int> vertex(int u) {
        auto result = add(-1, -1, NodeType::Vertex);
        vertexs_[u] = result;
        return {result, 1};
    }

    std::pair<int, int> add_edge(int u, int i) {
        auto comp = compress(u);
        auto result = add(comp.first, -1, NodeType::AddEdge);
        edges_[i] = result;
        return {result, comp.second};
    }

    std::pair<int, int> rake_merge(const std::vector<std::pair<int, int>>& seq) {
        if (seq.size() == 1) return seq.front();
        std::vector<std::pair<int, int>> lseq, rseq;
        int u = 0;
        for (auto& [_, s]: seq) u += s;
        for (auto& [i, s]: seq) (u > s ? lseq : rseq).emplace_back(i, s), u -= s * 2;
        auto lhs = rake_merge(lseq);
        auto rhs = rake_merge(rseq);
        return {add(lhs.first, rhs.first, NodeType::Rake), lhs.second + rhs.second};
    }

    std::pair<int, int> rake(int u) {
        std::vector<std::pair<int, int>> seq;
        for (int i = 0; auto e: graph_[u]) {
            if (i != 0) seq.emplace_back(add_edge(e.to, e.id));
            i++;
        }
        return rake_merge(seq);
    }

    int add(int left, int right, NodeType type) {
        int id = nodes_.size();
        if (left != -1) {
            nodes_[left].parent = id;
        }
        if (right != -1) {
            nodes_[right].parent = id;
        }
        nodes_.push_back({type, left, right, -1});
        return id;
    }

    int n_;
    Graph<T> graph_;
    std::vector<Node> nodes_;
    std::vector<int> vertexs_;
    std::vector<int> edges_;
    int root_;
};

}  //  namespace internal

template <class T>
StaticTopTree to_static_top_tree(int n, const std::vector<T>& edges, int root = 0) {
    return internal::StaticTopTreeBuilder(n, edges, root).static_top_tree();
}

}  //  namespace nono
