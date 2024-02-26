#define PROBLEM "https://yukicoder.me/problems/no/1002"

#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/tree/centroids.hpp"

namespace nono {

void solve() {
    int n, k;
    std::cin >> n >> k;
    std::vector<WeightedEdge<int>> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; i++) {
        int u, v, c;
        std::cin >> u >> v >> c;
        u--;
        v--;
        edges.emplace_back(u, v, c);
    }
    const auto graph = to_undirected_graph(n, edges);
    std::vector<bool> removed(n);
    long long ans = 0;
    for (auto centroid: centroids(graph)) {
        //  (1) (a, b)
        //     (a, -), (b, -), (a, b)
        //
        //  (2) (a, -)
        //     (a, b), (b, -)

        //  (a, b) a < b
        std::map<std::pair<int, int>, int> a;
        //  (a, *) aとなんか
        std::map<int, int> b;
        //  a only
        std::map<int, int> c;
        int d = 0;

        std::vector<std::pair<int, int>> path;
        auto dfs = [&](auto&& self, int u, int p, std::pair<int, int> color) -> void {
            path.push_back(color);
            for (const auto& e: graph[u]) {
                if (e.to == p || removed[e.to]) continue;
                auto new_color = color;
                if (new_color.first == -1) {
                    new_color.first = e.weight;
                    self(self, e.to, u, new_color);
                } else if (new_color.first != e.weight && new_color.second == -1) {
                    new_color.second = e.weight;
                    self(self, e.to, u, new_color);
                } else if (new_color.first == e.weight || new_color.second == e.weight) {
                    self(self, e.to, u, new_color);
                }
            }
        };

        for (const auto& e: graph[centroid]) {
            if (removed[e.to]) continue;
            path.clear();
            dfs(dfs, e.to, centroid, {e.weight, -1});
            for (auto [c1, c2]: path) {
                if (c1 == -1) continue;
                if (c2 == -1) {
                    //  (c1, *)
                    ans += b[c1];
                    //  (!c1, -)
                    ans += d - c[c1];
                } else {
                    if (c1 > c2) std::swap(c1, c2);
                    //  (c1, c2)
                    ans += a[std::pair(c1, c2)];
                    //  (c1, -)
                    ans += c[c1];
                    //  (c2, -)
                    ans += c[c2];
                    //  (-, -)
                    ans++;
                }
            }

            for (auto [c1, c2]: path) {
                if (c1 == -1) continue;
                if (c2 == -1) {
                    c[c1]++;
                    d++;
                } else {
                    if (c1 > c2) std::swap(c1, c2);
                    a[std::pair(c1, c2)]++;
                    b[c1]++;
                    b[c2]++;
                }
            }
        }

        removed[centroid] = true;
    }

    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
