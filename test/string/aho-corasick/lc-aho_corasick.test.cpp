// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/aho_corasick
#include <iostream>
#include <string>
#include <vector>

#include "nono/string/aho-corasick.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<std::string> words(n);
    for (auto& w: words) std::cin >> w;

    AhoCorasick<char> ac(words);
    auto nodes = ac.nodes();
    int total = nodes.size();

    std::cout << total << '\n';
    for (int v = 1; v < total; v++) {
        std::cout << nodes[v].parent << ' ' << nodes[v].link << '\n';
    }
    for (int i = 0; i < n; i++) {
        int cur = 0;
        for (char c: words[i]) cur = nodes[cur].to.at(c);
        std::cout << cur << " \n"[i + 1 == n];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    nono::solve();
}
