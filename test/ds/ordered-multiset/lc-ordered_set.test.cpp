#define PROBLEM "https://judge.yosupo.jp/problem/ordered_set"
#include <iostream>

#include "../../../nono/ds/ordered-multiset.hpp"

void solve() {
    int n, q;
    std::cin >> n >> q;
    nono::OrderedMultiSet<int> set;
    for (int i = 0; i < n; i++) {
        int v;
        std::cin >> v;
        set.insert(v);
    }
    while (q--) {
        int t, x;
        std::cin >> t >> x;
        if (t == 0) {
            if (set.contains(x)) continue;
            set.insert(x);
        } else if (t == 1) {
            set.erase(x);
        } else if (t == 2) {
            x--;
            if (x < set.size()) {
                std::cout << set.kth(x) << '\n';
            } else {
                std::cout << -1 << '\n';
            }
        } else if (t == 3) {
            std::cout << set.rank(x + 1) << '\n';
        } else if (t == 4) {
            auto ans = set.predecessor(x + 1);
            std::cout << (ans ? *ans : -1) << '\n';
        } else {
            auto ans = set.successor(x - 1);
            std::cout << (ans ? *ans : -1) << '\n';
        }
    }
}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    solve();
}