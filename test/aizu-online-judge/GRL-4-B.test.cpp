#define IGNORE
#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/library/5/GRL/4/GRL_4_B"
#include <iostream>
#include <vector>

#include "graph/graph.hpp"
#include "graph/edge.hpp"
#include "graph/topological-sort.hpp"

namespace nono {

struct E {
    int from;
    int to;
};

void solve() {
    int n, m;
    std::cin >> n >> m;
    Graph<E> graph(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        graph.add_edge(E(u, v));
    }

    for (int i: topological_sort(graph)) {
        std::cout << i << std::endl;
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
