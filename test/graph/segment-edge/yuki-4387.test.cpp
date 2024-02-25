#define PROBLEM "https://yukicoder.me/problems/4387"
#include <iostream>
#include <vector>

#include "nono/graph/segment-edge.hpp"
#include "nono/utility/compressor.hpp"
#include "nono/graph/strongly-connected-components.hpp"

namespace nono {

void solve() {
    int n, a, b;
    std::cin >> n >> a >> b;
    std::vector<int> x(n);
    for (int i = 0; i < n; i++) std::cin >> x[i];
    Compressor compressor(x);
    std::vector<SegmentEdge<int>> edges;
    //  [x[i] + a, x[i] + b + 1)
    //  [x[i] - b, x[i] - a + 1)
    for (int i = 0; i < n; i++) {
        {
            int left = compressor.compress(x[i] + a);
            int right = compressor.compress(x[i] + b + 1);
            edges.emplace_back(std::pair(i, i + 1), std::pair(left, right), 1);
        }
        {
            int left = compressor.compress(x[i] - b);
            int right = compressor.compress(x[i] - a + 1);
            edges.emplace_back(std::pair(i, i + 1), std::pair(left, right), 1);
        }
    }
    auto [graph, mapping] = to_graph(n, edges);
    std::vector<int> ans(n);
    for (auto&& group: strongly_connected_components(graph)) {
        int count = 0;
        for (auto u: group) {
            if (mapping[u] != -1) count++;
        }
        for (auto u: group) {
            if (mapping[u] != -1) ans[mapping[u]] = count;
        }
    }
    for (int i = 0; i < n; i++) {
        std::cout << ans[i] << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
