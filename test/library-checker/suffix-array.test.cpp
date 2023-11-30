#define PROBLEM "https://judge.yosupo.jp/problem/suffixarray"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>

#include "string/suffix-array.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    SuffixArray sa(s);
    int n = sa.size();
    for (int i = 1; i < n; i++) {
        std::cout << sa[i] << (i + 1 < n ? ' ' : '\n');
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
