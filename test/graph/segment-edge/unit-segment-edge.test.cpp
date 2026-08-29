// competitive-verifier: STANDALONE
#include <cassert>
#include <random>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/dijkstra.hpp"
#include "nono/graph/segment-edge.hpp"

namespace nono {

void solve() {
    std::mt19937 mt(123);
    constexpr int n = 8;

    for (int iter = 0; iter < 30; iter++) {
        std::uniform_int_distribution<int> pos(0, n);
        std::uniform_int_distribution<int> weight(1, 20);

        std::vector<SegmentEdge<int>> seg_edges;
        std::vector<WeightedEdge<int>> brute_edges;

        int m = 5;
        for (int i = 0; i < m; i++) {
            int a = pos(mt), b = pos(mt);
            if (a > b) std::swap(a, b);
            if (a == b) continue;
            int c = pos(mt), d = pos(mt);
            if (c > d) std::swap(c, d);
            if (c == d) continue;
            int w = weight(mt);
            seg_edges.push_back({{a, b}, {c, d}, w});
            for (int u = a; u < b; u++) {
                for (int v = c; v < d; v++) {
                    brute_edges.emplace_back(u, v, w);
                }
            }
        }

        auto [seg_graph, mapping] = to_segment_graph(n, seg_edges);
        auto brute_graph = to_directed_graph(n, brute_edges);

        //  mapping[j] = i <=> 圧縮グラフの頂点jは元のグラフの頂点iに対応する
        std::vector<int> vertex_of(n, -1);
        for (int j = 0; j < (int)mapping.size(); j++) {
            if (mapping[j] != -1) vertex_of[mapping[j]] = j;
        }
        for (int i = 0; i < n; i++) assert(vertex_of[i] != -1);

        for (int s = 0; s < n; s++) {
            auto seg_dist = dijkstra(seg_graph, vertex_of[s]);
            auto brute_dist = dijkstra(brute_graph, s);
            for (int t = 0; t < n; t++) {
                assert(seg_dist.invalid(vertex_of[t]) == brute_dist.invalid(t));
                if (!brute_dist.invalid(t)) {
                    assert(seg_dist.dist(vertex_of[t]) == brute_dist.dist(t));
                }
            }
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
