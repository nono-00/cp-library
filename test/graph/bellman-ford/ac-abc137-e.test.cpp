#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc137/tasks/abc137_e"
#include <algorithm>
#include <iostream>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/bellman-ford.hpp"

namespace nono {

void solve() {
    int n, m, p;
    std::cin >> n >> m >> p;
    std::vector<WeightedEdge<long long>> edges(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        u--;
        v--;
        edges.emplace_back(u, v, p - w);
    }
    auto result = bellman_ford(n, edges, 0);

    if (result.invalid(n - 1)) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << std::max(0LL, -result.dist(n - 1)) << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
