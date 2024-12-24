#define PROBLEM "https://judge.yosupo.jp/problem/enumerate_palindromes"
#include <iostream>
#include <string>

#include "../../../nono/string/manacher.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    int n = s.size();
    auto result = manacher(s);
    for (int i = 0; i < n; i++) {
        std::cout << 2 * result.radius(i) - 1;
        if (i + 1 < n) {
            std::cout << ' ' << 2 * result.radius(i, i + 1) << ' ';
        } else {
            std::cout << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
