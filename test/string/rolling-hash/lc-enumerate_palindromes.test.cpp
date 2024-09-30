#define PROBLEM "https://judge.yosupo.jp/problem/enumerate_palindromes"
#include <algorithm>
#include <iostream>
#include <string>

#include "nono/algo/binary-search.hpp"
#include "nono/string/rolling-hash.hpp"

namespace nono {

void solve() {
    std::string s;
    std::cin >> s;
    std::string t;
    t.push_back('#');
    for (auto c: s) t.push_back(c), t.push_back('#');
    auto u = t;
    std::reverse(u.begin(), u.end());
    int n = t.size();
    auto pos = [&](int i) {
        return n - i - 1;
    };
    RollingHash rh(1421);
    auto t_table = rh.table(t);
    auto u_table = rh.table(u);

    std::vector<int> ans(n - 2);
    for (int i = 1; i + 1 < n; i++) {
        auto pred = [&](int len) -> bool {
            int j = pos(i);
            if (!(0 <= i - len && i + len < n)) return false;
            if (!(0 <= j - len && j + len < n)) return false;
            return rh.hash(t_table, i - len, i + len + 1) == rh.hash(u_table, j - len, j + len + 1);
        };
        int len = binary_search(0, std::min(i + 1, n - i), pred);
        std::cout << len - 1 << " \n"[i + 2 == n];
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
