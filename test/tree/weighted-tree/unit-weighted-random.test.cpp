// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/bfs.hpp"
#include "nono/tree/weighted-tree.hpp"

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

        WeightedTree<int> tree(n, edges, 0);

        for (int x = 0; x < n; x++) {
            assert(tree.weighted_depth(x) == bfs(graph, 0).dist(x));
        }

        for (int x = 0; x < n; x++) {
            int expected = 0;
            for (int y = 0; y < n; y++) {
                if (tree.lca(x, y) == x) {
                    expected = std::max(expected, bfs(graph, x).dist(y));
                }
            }
            assert(tree.weighted_height(x) == expected);
        }

        int trial = 1 + (int)(rng() % 20);
        for (int t = 0; t < trial; t++) {
            int u = (int)(rng() % n);
            int v = (int)(rng() % n);
            assert(tree.weighted_dist(u, v) == bfs(graph, u).dist(v));
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
