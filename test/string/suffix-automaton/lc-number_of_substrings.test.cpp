#define PROBLEM "https://judge.yosupo.jp/problem/number_of_substrings"
#include <iostream>
#include <string>

#include "../../../nono/string/suffix-automaton.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    SuffixAutomaton sa;
    for (auto c: s) sa.add(c);
    std::cout << sa.subseq() << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

