#define PROBLEM "https://judge.yosupo.jp/problem/longest_common_substring"
#include <iostream>
#include <string>

#include "../../../nono/string/suffix-automaton.hpp"

namespace nono {

void solve() {
    std::string s, t;
    std::cin >> s >> t;
    SuffixAutomaton sa;
    sa.add(s);
    auto [a, b, c, d] = sa.lcs(t);
    std::cout << a << ' ' << b << ' ' << c << ' ' << d << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}


