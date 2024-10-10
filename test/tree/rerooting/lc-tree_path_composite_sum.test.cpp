#define PROBLEM "https://judge.yosupo.jp/problem/tree_path_composite_sum"
#include <iostream>
#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/math/modint.hpp"
#include "nono/tree/rerooting.hpp"

namespace nono {

using Mint = Modint998244353;

struct ReRootingInfo {
    struct Value {
        Value(Mint sum, Mint num = 1): sum(sum), num(num) {}
        Mint sum;
        Mint num;
    };
    using Edge = std::pair<Mint, Mint>;
    static Value e() {
        return Value{0, 0};
    }
    static Value merge(Value lhs, Value rhs) {
        return Value{lhs.sum + rhs.sum, lhs.num + rhs.num};
    }
    static Value add_edge(Edge edge, Value vertex) {
        return Value{edge.first * vertex.sum + edge.second * vertex.num, vertex.num};
    }
    static Value add_vertex(Value lhs, Value rhs) {
        return Value{lhs.sum + rhs.sum, lhs.num + rhs.num};
    }
};

void solve() {
    int n;
    std::cin >> n;
    std::vector<Mint> vertex(n);
    for (int i = 0; i < n; i++) std::cin >> vertex[i];
    std::vector<WeightedEdge<std::pair<Mint, Mint>>> edges(n - 1);
    for (auto& [u, v, w]: edges) std::cin >> u >> v >> w.first >> w.second;
    auto result = rerooting<ReRootingInfo>(to_undirected_graph(n, edges), {vertex.begin(), vertex.end()});
    for (int i = 0; i < n; i++) std::cout << result[i].sum << " \n"[i + 1 == n];
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
