#define PROBLEM "https://judge.yosupo.jp/problem/sum_of_floor_of_linear"

#include <bits/stdc++.h>

#include "../../../nono/math/floor-sum.hpp"

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    int t;
    std::cin >> t;
    while (t--) {
        long long n, m, a, b;
        std::cin >> n >> m >> a >> b;
        std::cout << nono::floor_sum(n, m, a, b) << '\n';
    }
}
