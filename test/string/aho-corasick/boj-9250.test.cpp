#include <iostream>
#include <string>

#include "../../../nono/string/aho-corasick.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    std::vector<std::string> words(n);
    for (auto& word: words) std::cin >> word;
    AhoCorasick ahocora(words);
    int q;
    std::cin >> q;
    while (q--) {
        std::string s;
        std::cin >> s;
        std::cout << (ahocora.contains(s) ? "YES" : "NO") << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
