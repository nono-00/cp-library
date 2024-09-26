#define PROBLEM "https://judge.yosupo.jp/problem/two_sat"
#include <iostream>
#include <vector>

#include "nono/algo/two-sat.hpp"

namespace nono {

void solve() {
    {
        char c;
        std::string s;
        std::cin >> c >> s;
    }
    int n, m;
    std::cin >> n >> m;
    auto encode = [&](int x) {
        return x < 0 ? ~(-x - 1) : (x - 1);
    };
    TwoSat sat(n);
    for (int i = 0; i < m; i++) {
        int u, v, _;
        std::cin >> u >> v >> _;
        sat.add_clause(encode(u), encode(v));
    }
    if (sat.satisfiable()) {
        std::cout << "s SATISFIABLE" << '\n';
        auto ans = sat.answer();
        std::cout << "v ";
        for (int i = 0; i < n; i++) std::cout << (ans[i] ? i + 1 : -(i + 1)) << ' ';
        std::cout << 0 << '\n';
    } else {
        std::cout << "s UNSATISFIABLE" << '\n';
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
