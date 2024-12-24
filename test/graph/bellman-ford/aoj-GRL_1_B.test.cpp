#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/1/GRL_1_B"
#include <iostream>
#include <vector>

#include "../../../nono/graph/base.hpp"
#include "../../../nono/graph/bellman-ford.hpp"

namespace nono {

void solve() {
    int n, m, s;
    std::cin >> n >> m >> s;
    std::vector<WeightedEdge<long long>> edges(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.emplace_back(u, v, w);
    }
    auto result = bellman_ford(n, edges, s);

    if (result.has_negative_cycle()) {
        std::cout << "NEGATIVE CYCLE" << '\n';
    } else {
        for (int i = 0; i < n; i++) {
            if (result.invalid(i)) {
                std::cout << "INF" << '\n';
            } else {
                std::cout << result.dist(i) << '\n';
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
