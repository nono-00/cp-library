// https://www.acmicpc.net/problem/16907
#include <iostream>
#include <string>

#include "nono/string/suffix-automaton.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    SuffixAutomaton sa;
    for (auto c: s) {
        if (c == '?') {
            std::cout << sa.subseq() << '\n';
        } else {
            sa.add(c);
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}


