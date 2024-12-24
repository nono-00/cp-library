// https://codeforces.com/contest/2051/problem/F
#include <iostream>

#include "../../../nono/ds/interval-set.hpp"

namespace nono {

void solve() {
    int n;
    std::cin >> n;
    IntervalSet set;
    int q;
    std::cin >> q;
    while (q--) {
        int l, r, k;
        std::cin >> l >> r >> k;
        l--;
        if (k == 1) {
            set.add(l, r);
        } else {
            set.erase(l, r);
        }
        std::cout << n - set.size() << '\n';
    }
}

}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

