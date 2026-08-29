// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/bfs.hpp"
#include "nono/tree/is-tree.hpp"
#include "nono/tree/tree-compressor.hpp"

namespace nono {

void solve() {
    std::mt19937 rng(20260821);

    const int iteration = 200;
    for (int iter = 0; iter < iteration; iter++) {
        int n = 1 + (int)(rng() % 30);
        std::vector<WeightedEdge<int>> edges;
        edges.reserve(n - 1);
        for (int i = 1; i < n; i++) {
            int p = (int)(rng() % i);
            int weight = 1 + (int)(rng() % 100);
            edges.emplace_back(p, i, weight);
        }
        auto graph = to_undirected_graph(n, edges);
        assert(is_tree(graph));

        WeightedTreeCompressor<int> tc(graph);

        int k = 1 + (int)(rng() % n);
        std::vector<int> vertices;
        for (int i = 0; i < k; i++) vertices.push_back((int)(rng() % n));

        auto result = tc.compress(vertices);
        const auto& compressed = result.graph;
        const auto& mapping = result.mapping;

        assert(is_tree(compressed));
        assert((int)mapping.size() == compressed.size());

        for (int v: vertices) {
            assert(std::find(mapping.begin(), mapping.end(), v) != mapping.end());
        }

        for (int u = 0; u < compressed.size(); u++) {
            auto from = bfs(graph, mapping[u]);
            for (const auto& e: compressed[u]) {
                assert(e.weight == from.dist(mapping[e.to]));
            }
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
