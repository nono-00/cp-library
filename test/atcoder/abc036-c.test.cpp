#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc036/tasks/abc036_c"
#include <iostream>
#include <vector>

#include "utility/compressor.hpp"

void solve() {
    using namespace nono;
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++) std::cin >> a[i];
    Compressor compresser(a);
    for (int v: a) {
        std::cout << compresser.compress(v) << '\n';
    }
}

int main() {
    solve();
}
