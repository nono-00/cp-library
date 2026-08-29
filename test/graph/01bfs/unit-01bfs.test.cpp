// competitive-verifier: STANDALONE
#include <cassert>
#include <random>
#include <vector>

#include "nono/graph/01bfs.hpp"
#include "nono/graph/base.hpp"
#include "nono/graph/dijkstra.hpp"

namespace nono {

void solve() {
    std::mt19937 mt(7);
    constexpr int n = 30;

    for (int iter = 0; iter < 30; iter++) {
        std::vector<WeightedEdge<int>> edges;
        std::uniform_int_distribution<int> vtx(0, n - 1);
        std::uniform_int_distribution<int> weight(0, 1);
        int m = n * 3;
        for (int i = 0; i < m; i++) {
            edges.emplace_back(vtx(mt), vtx(mt), weight(mt));
        }
        auto graph = to_directed_graph(n, edges);

        //  単一始点: bfs01とdijkstra(既にverify済み)の結果を突き合わせる
        int source = vtx(mt);
        auto b = bfs01(graph, source);
        auto d = dijkstra(graph, source);
        for (int v = 0; v < n; v++) {
            assert(b.invalid(v) == d.invalid(v));
            if (!b.invalid(v)) {
                assert(b.dist(v) == d.dist(v));
                auto path = b.path(v);
                assert(path.front() == source && path.back() == v);
                //  pathが辺の連なりとして実在し、重みの合計がdist(v)と一致することを確認
                int sum = 0;
                for (int i = 0; i + 1 < (int)path.size(); i++) {
                    int best = -1;
                    for (const auto& e: graph[path[i]]) {
                        if (e.to == path[i + 1] && (best == -1 || e.weight < best)) {
                            best = e.weight;
                        }
                    }
                    assert(best != -1);
                    sum += best;
                }
                assert(sum == b.dist(v));
            } else {
                assert(b.path(v).empty());
            }
        }
        assert(b.raw() == d.raw());

        //  複数始点
        std::vector<int> sources = {vtx(mt), vtx(mt), vtx(mt)};
        auto bm = bfs01(graph, sources);
        for (int v = 0; v < n; v++) {
            int expected = bfs01_internal::Bfs01Result<int>::UNREACHABLE;
            for (int s: sources) {
                auto ds = dijkstra(graph, s);
                if (!ds.invalid(v)) expected = std::min(expected, ds.dist(v));
            }
            if (expected == bfs01_internal::Bfs01Result<int>::UNREACHABLE) {
                assert(bm.invalid(v));
            } else {
                assert(!bm.invalid(v));
                assert(bm.dist(v) == expected);
            }
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
