#define PROBLEM "https://judge.yosupo.jp/problem/zalgorithm"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "string/z-algorithm.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    int n = s.size();
    auto z_values = z_algorithm(s);
    for (int i = 0; i < n; i++) {
        std::cout << z_values[i] << (i + 1 == n ? '\n' : ' ');
    }
}

}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
