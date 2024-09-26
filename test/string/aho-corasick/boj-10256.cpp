#include <algorithm>
#include <iostream>
#include <string>

#include "nono/string/aho-corasick.hpp"

namespace nono {

void solve() {
    int n, m;
    std::cin >> n >> m;
    std::string s, t;
    std::cin >> s >> t;
    std::vector<std::string> words;
    for (int l = 0; l < m; l++) {
        for (int r = l + 1; r <= m; r++) {
            auto lhs = t.substr(0, l);
            auto mhs = t.substr(l, r - l);
            auto rhs = t.substr(r, m);
            std::reverse(mhs.begin(), mhs.end());
            words.push_back(lhs + mhs + rhs);
        }
    }
    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());
    AhoCorasick aho(words);
    std::cout << aho.count<long long>(s) << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    int t;
    std::cin >> t;
    while (t--) nono::solve();
}
