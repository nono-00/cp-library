#include <iostream>
#include <string>

#include "../../../nono/string/eertree.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    Eertree tree;
    tree.add(std::vector<char>(s.begin(), s.end()));
    int m = tree.size();
    auto nodes = tree.nodes();
    auto freq = tree.freq();
    long long ans = 0;
    for (int i = 0; i < m; i++) {
        ans = std::max(ans, (long long)freq[i + 2] * nodes[i + 2].len);
    }
    std::cout << ans << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
