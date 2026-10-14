// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/0343
#include <iomanip>
#include <iostream>
#include <vector>

#include "nono/ds/ordered-map.hpp"

namespace nono {

void solve() {
    OrderedMap<std::pair<long long, int>, int> map;
    int n, c;
    std::cin >> n >> c;
    std::vector<long long> score(n);
    for (int i = 0; i < n; i++) {
        map.set({0, i}, 0);
    }
    while (c--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int i;
            long long p;
            std::cin >> i >> p;
            i--;
            map.erase({score[i], i});
            score[i] -= p;
            map.set({score[i], i}, 0);
        } else {
            int i;
            std::cin >> i;
            i--;
            auto [key, value] = map.kth(i);
            std::cout << key.second + 1 << ' ' << -key.first << std::endl;
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(false);
    std::cout << std::fixed << std::setprecision(15);

    int t = 1;
    while (t--) nono::solve();
}
