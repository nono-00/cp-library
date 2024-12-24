#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/abc313/tasks/abc313_e"
#include <algorithm>
#include <iostream>
#include <string>

#include "../../../nono/math/modint.hpp"
#include "../../../nono/utility/run-length-encoding.hpp"

namespace nono {

using Mint = nono::Modint998244353;

void solve() {
    int n;
    std::cin >> n;
    std::string s;
    std::cin >> s;
    for (int i = 0; i + 1 < n; i++) {
        if (s[i] != '1' && s[i + 1] != '1') {
            std::cout << -1 << std::endl;
            return;
        }
    }

    auto rle = run_length_encoding(s);

    Mint count = 0;
    int prev = 0;
    std::ranges::reverse(rle);
    for (auto [c, v]: rle) {
        if (c == '1') {
            count += (prev - 1) * count;
            count += v;
        } else {
            count += 1;
            prev = c - '0';
        }
    }
    std::cout << (count - 1) << std::endl;
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
