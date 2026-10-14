#pragma once

#include <cassert>
#include <iterator>
#include <ranges>
#include <vector>

namespace nono {

//  # EdgeBase
//  fromからtoへ向かう、重みweightの辺。weightを省略すると1になる
//  aliasとしてEdge, WeightedEdgeがあるのでそちらを使うこと
template <class T>
struct EdgeBase {
    int from;
    int to;
    T weight;
    EdgeBase() {}
    EdgeBase(int from, int to, T weight = 1): from(from), to(to), weight(weight) {}
};

//  # Edge
//  重みがintの辺。重みなしのグラフではweightを省略して使う
using Edge = EdgeBase<int>;

//  # WeightedEdge<T>
//  重みがT型の辺
template <class T>
using WeightedEdge = EdgeBase<T>;

//  # Graph
//  構築した後は辺を変更しない静的なグラフ。CSR形式で隣接リストを持つ
//  to_undirected_graph, to_directed_graph で作る
template <class T>
class Graph {
    struct Edge_ {
        int to;
        T weight;
        int id;
    };

    using iterator = std::vector<Edge_>::iterator;
    using const_iterator = std::vector<Edge_>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    template <class U>
    friend Graph<U> to_undirected_graph(int n, const std::vector<EdgeBase<U>>& edges);
    template <class U>
    friend Graph<U> to_directed_graph(int n, const std::vector<EdgeBase<U>>& edges);

    //  # operator[](i)
    //  頂点iから出る辺の列を返す。各要素は to(行き先), weight(重み), id(edgesでの添字) を持つ
    //  無向グラフでは、1本の辺が両端点の列に同じidで入る
    //  0 <= i < size() でないと壊れる
    subrange operator[](int i) {
        assert(0 <= i && i < n_);
        return std::ranges::subrange(edges_.begin() + indptr_[i], edges_.begin() + indptr_[i + 1]);
    }

    //  # operator[](i)
    //  constなグラフに対する operator[]。内容は同じ
    const_subrange operator[](int i) const {
        assert(0 <= i && i < n_);
        return std::ranges::subrange(edges_.begin() + indptr_[i], edges_.begin() + indptr_[i + 1]);
    }

    //  # size()
    //  |V|
    int size() const {
        return n_;
    }

    //  # edge_size()
    //  |E|
    int edge_size() const {
        return m_;
    }

    //  # is_directed()
    //  有向グラフか
    bool is_directed() const {
        return directed_;
    }

    //  # is_undirected()
    //  無向グラフか
    bool is_undirected() const {
        return !is_directed();
    }

  private:
    //  publicではないので、外部からは呼び出せない
    //  to_undirected_graph, to_directed_graphを代わりに使用すること
    Graph(int n, const std::vector<EdgeBase<T>>& edges, bool directed)
        : n_(n),
          m_(edges.size()),
          indptr_(n_ + 1),
          edges_(directed ? edges.size() : 2 * edges.size()),
          directed_(directed) {
        for (const auto& e: edges) {
            assert(0 <= e.from && e.from < n_);
            assert(0 <= e.to && e.to < n_);
            indptr_[e.from + 1]++;
            if (!directed_) indptr_[e.to + 1]++;
        }
        for (int i = 0; i < n_; i++) {
            indptr_[i + 1] += indptr_[i];
        }
        auto index = indptr_;
        for (int i = 0; i < std::ssize(edges); i++) {
            const auto& e = edges[i];
            edges_[index[e.from]++] = Edge_(e.to, e.weight, i);
            if (!directed_) edges_[index[e.to]++] = Edge_(e.from, e.weight, i);
        }
    }

    int n_;
    int m_;
    std::vector<int> indptr_;
    std::vector<Edge_> edges_;
    bool directed_;
};

//  # to_undirected_graph(n, edges)
//  頂点数がn、辺がedgesの無向グラフを作る
//  edgesの端点が [0, n) でないと壊れる
//  O(n + m)
template <class T>
Graph<T> to_undirected_graph(int n, const std::vector<EdgeBase<T>>& edges) {
    return Graph<T>(n, edges, false);
}

//  # to_directed_graph(n, edges)
//  頂点数がn、辺がedgesの有向グラフを作る
//  edgesの端点が [0, n) でないと壊れる
//  O(n + m)
template <class T>
Graph<T> to_directed_graph(int n, const std::vector<EdgeBase<T>>& edges) {
    return Graph<T>(n, edges, true);
}

}  //  namespace nono
