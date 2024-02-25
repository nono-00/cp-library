#define PROBLEM "https://judge.yosupo.jp/problem/enumerate_palindromes"
#include <iostream>
#include <string>

#include "nono/string/manacher.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    std::string t;
    for (auto c: s) {
        t.push_back(c);
        t.push_back('#');
    }
    t.pop_back();

    const auto res = manacher(t);
    for (int i = 0, size = res.size(); i < size; i++) {
        if (i % 2 == 0) {
            std::cout << 2 * ((res[i] - 1) / 2) + 1;
        } else {
            std::cout << 2 * ((res[i]) / 2);
        }
        if (i + 1 != size) {
            std::cout << ' ';
        }
    }
    std::cout << '\n';
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
