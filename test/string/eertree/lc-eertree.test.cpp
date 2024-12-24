#define PROBLEM "https://judge.yosupo.jp/problem/eertree"
#include <iostream>
#include <string>

#include "../../../nono/string/eertree.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    int n = s.size();
    Eertree tree;
    tree.add(std::vector<char>(s.begin(), s.end()));
    int m = tree.size();
    std::cout << m << '\n';
    auto nodes = tree.nodes();
    auto node_id = tree.node_id();
    for (int i = 0; i < m; i++) {
        std::cout << nodes[i + 2].parent - 1 << ' ' << nodes[i + 2].link - 1 << '\n';
    }
    for (int i = 0; i < n; i++) {
        std::cout << node_id[i] - 1 << " \n"[i + 1 == n];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
