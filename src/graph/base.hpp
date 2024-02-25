#pragma once

#include <cassert>
#include <iterator>
#include <ranges>
#include <vector>

namespace nono {

//  グラフの辺を表す基本的な情報を保持する構造体
//  直接利用することは少ない
//  aliasとしてEdge, WeightedEdgeがあるのでそちらの利用を推奨
template <class T>
struct EdgeBase {
    int from;  //  辺の始点
    int to;    //  辺の終点
    T weight;  //  辺の重み
    EdgeBase() {}
    EdgeBase(int from, int to, T weight = 1): from(from), to(to), weight(weight) {}
};

//  重みを持たない辺の型
//  便宜上intを持たせている
using Edge = EdgeBase<int>;
//  任意の型の重みを持つ辺の型
template <class T>
using WeightedEdge = EdgeBase<T>;

//  グラフを表すクラス
template <class T>
class Graph {
    //  グラフの辺を表す内部構造体
    struct Edge {
        int to;    //  辺の終点
        T weight;  //  辺の重み
        int id;    //  辺の識別子
    };

    using iterator = std::vector<Edge>::iterator;
    using const_iterator = std::vector<Edge>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    template <class U>
    friend Graph<U> to_undirected_graph(int n, const std::vector<EdgeBase<U>>& edges);
    template <class U>
    friend Graph<U> to_directed_graph(int n, const std::vector<EdgeBase<U>>& edges);

    subrange operator[](int i) {
        assert(0 <= i && i < n_);
        return std::ranges::subrange(edges_.begin() + indptr_[i], edges_.begin() + indptr_[i + 1]);
    }
    const_subrange operator[](int i) const {
        assert(0 <= i && i < n_);
        return std::ranges::subrange(edges_.begin() + indptr_[i], edges_.begin() + indptr_[i + 1]);
    }
    //  グラフの頂点数を返す関数
    int size() const {
        return n_;
    }

  private:
    //  コンストラクタ
    //  publicではないので、外部からは呼び出せない
    //  to_undirected_graph, to_directed_graphを代わりに使用すること
    Graph(int n, const std::vector<EdgeBase<T>>& edges, bool directed)
        : n_(n),
          indptr_(n + 1),
          edges_(directed ? edges.size() : 2 * edges.size()) {
        for (const auto& edge: edges) {
            assert(0 <= edge.from && edge.from < n_);
            assert(0 <= edge.to && edge.to < n_);
            indptr_[edge.from + 1]++;
            if (!directed) indptr_[edge.to + 1]++;
        }
        for (int i = 0; i < n_; i++) {
            indptr_[i + 1] += indptr_[i];
        }
        auto index = indptr_;
        for (int i = 0; i < std::ssize(edges); i++) {
            const auto& edge = edges[i];
            edges_[index[edge.from]++] = Edge(edge.to, edge.weight, i);
            if (!directed) edges_[index[edge.to]++] = Edge(edge.from, edge.weight, i);
        }
    }

    int n_;                    //  グラフの頂点数
    std::vector<int> indptr_;  //  隣接リストのインデックス
    std::vector<Edge> edges_;  //  エッジのリスト
};

//  無向グラフを作成する関数
template <class T>
Graph<T> to_undirected_graph(int n, const std::vector<EdgeBase<T>>& edges) {
    return Graph<T>(n, edges, false);
}

//  有向グラフを作成する関数
template <class T>
Graph<T> to_directed_graph(int n, const std::vector<EdgeBase<T>>& edges) {
    return Graph<T>(n, edges, true);
}

}  //  namespace nono
