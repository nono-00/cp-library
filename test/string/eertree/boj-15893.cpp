#include <iostream>
#include <string>
#include <vector>

#include "nono/string/eertree.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    Eertree tree;
    std::vector<decltype(Eertree<char>{}.nodes())> nodes;
    for (int i = 0; i < n; i++) {
        std::string s;
        std::cin >> s;
        Eertree tree;
        tree.add(std::vector(s.begin(), s.end()));
        nodes.push_back(tree.nodes());
    }

    int ans = 0;
    auto dfs = [&](auto&& self, const std::vector<int>& cur) -> void {
        ans = std::max(ans, nodes[0][cur[0]].len);
        std::vector<int> nxt(n);
        for (char c = 'a'; c <= 'z'; c++) {
            bool ok = true;
            for (int i = 0; i < n; i++) {
                if (!nodes[i][cur[i]].to.contains(c)) {
                    ok = false;
                    break;
                }
                nxt[i] = nodes[i][cur[i]].to[c];
            }
            if (ok) self(self, nxt);
        }
    };
    dfs(dfs, std::vector<int>(n, 0));
    dfs(dfs, std::vector<int>(n, 1));
    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
