#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/0343"
#include <iomanip>
#include <iostream>
#include <vector>

#include "nono/ds/splay-tree.hpp"

namespace nono {

void solve() {
    SplayTree<std::pair<long long, int>, int> splay;
    int n, c;
    std::cin >> n >> c;
    std::vector<long long> score(n);
    for (int i = 0; i < n; i++) {
        splay.insert({0, i}, 0);
    }
    while (c--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            int i;
            long long p;
            std::cin >> i >> p;
            i--;
            splay.erase({score[i], i});
            score[i] -= p;
            splay.insert({score[i], i}, 0);
        } else {
            int i;
            std::cin >> i;
            i--;
            auto [key, value] = splay.kth_element(i);
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
