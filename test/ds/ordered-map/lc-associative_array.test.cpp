#define PROBLEM "https://judge.yosupo.jp/problem/associative_array"
#include <iostream>

#include "../../../nono/ds/ordered-map.hpp"

void solve() {
    int q;
    std::cin >> q;
    nono::OrderedMap<long long, long long> map;
    while (q--) {
        int t;
        long long k;
        std::cin >> t >> k;
        if (t == 0) {
            long long v;
            std::cin >> v;
            map.set(k, v);
        } else {
            if (map.contains(k)) {
                std::cout << map.get(k) << '\n';
            } else {
                std::cout << 0 << '\n';
            }
        }
    }
}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    solve();
}