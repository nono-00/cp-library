#define PROBLEM "https://judge.yosupo.jp/problem/suffixarray"
#include <iostream>
#include <string>

#include "nono/string/suffix-array.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    int n = s.size();
    auto sa = suffix_array(s);
    for (int i = 0; i < n; i++) {
        std::cout << sa[i] << " \n"[i + 1 == n];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
