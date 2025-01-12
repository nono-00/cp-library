#pragma once

#include <utility>
#include <vector>

#include "./base.hpp"

namespace nono {

///  # Segment Edge
///  from := [a, b)
///  to := [c, d)
///  Segment Edge := { (u, v, w) | u in [a, b), v in [c, d), w := weight }
template <class T>
struct SegmentEdge {
    std::pair<int, int> from;
    std::pair<int, int> to;
    T weight;
};

///  # to graph(n, segment edges)
///  区間辺集合から有向グラフを構築する
///
///  返り値のstd::vector<int>は構築後のグラフの頂点に
///  対応する元のグラフの頂点を表す
///
///  mapping: vertex(segment-graph) -> vertex(original-graph)
///
///  mapping[i] = -1ならば対応する頂点は存在しない
///  元のグラフにおける頂点iに対応する頂点はmapping[j] = iを満たすj
///
///  作られたグラフの頂点数が3 * n + mであることに注意
template <class T>
std::pair<Graph<T>, std::vector<int>> to_graph(int n, const std::vector<SegmentEdge<T>>& seg_edges) {
    int m = seg_edges.size();
    std::vector<WeightedEdge<T>> edges;
    //  補正用
    auto from = [n](int id) {
        return (n <= id ? id : id + 2 * n);
    };
    auto to = [](int id) {
        return id;
    };
    //  上から中央へ
    for (int u = n - 1; u > 0; u--) {
        edges.emplace_back(to(u), to(2 * u), 0);
        edges.emplace_back(to(u), to(2 * u + 1), 0);
    }
    //  中央から下へ
    for (int u = n - 1; u > 0; u--) {
        edges.emplace_back(from(2 * u), from(u), 0);
        edges.emplace_back(from(2 * u + 1), from(u), 0);
    }
    //  辺に対応する頂点を作って適切に辺を貼る
    for (int i = 0; i < m; i++) {
        int edge_id = i + 3 * n;
        {
            auto [left, right] = seg_edges[i].from;
            for (left += n, right += n; left < right; left >>= 1, right >>= 1) {
                if (left & 1) {
                    edges.emplace_back(from(left++), edge_id, 0);
                }
                if (right & 1) {
                    edges.emplace_back(from(--right), edge_id, 0);
                }
            }
        }
        {
            auto [left, right] = seg_edges[i].to;
            for (left += n, right += n; left < right; left >>= 1, right >>= 1) {
                if (left & 1) {
                    edges.emplace_back(edge_id, to(left++), seg_edges[i].weight);
                }
                if (right & 1) {
                    edges.emplace_back(edge_id, to(--right), seg_edges[i].weight);
                }
            }
        }
    }
    std::vector<int> mapping(3 * n + m, -1);
    for (int i = 0; i < n; i++) {
        mapping[i + n] = i;
    }
    return {to_directed_graph(3 * n + m, edges), mapping};
}

}  //  namespace nono
