#pragma once

#include <utility>
#include <vector>

#include "./base.hpp"

namespace nono {

//  # SegmentEdge
//  区間辺。from = [a, b) の各頂点から to = [c, d) の各頂点へ、重みweightの辺を張ることを表す
//  { (u, v, weight) | u in [a, b), v in [c, d) }
template <class T>
struct SegmentEdge {
    std::pair<int, int> from;
    std::pair<int, int> to;
    T weight;
};

//  # to_segment_graph(n, seg_edges)
//  区間辺の集合から、元の頂点どうしの距離が変わらない有向グラフを作る。nは元の頂点数
//  first  : 作ったグラフ。頂点数は 3 * n + m (m: 区間辺の数)
//  second : 作ったグラフの頂点から元の頂点への対応。-1なら対応する元の頂点は無い
//  元の頂点iは、作ったグラフの頂点 i + n に対応する
//  各区間が 0 <= left <= right <= n でないと壊れる
//  辺の数は O(n + m log n)
template <class T>
std::pair<Graph<T>, std::vector<int>> to_segment_graph(int n, const std::vector<SegmentEdge<T>>& seg_edges) {
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
