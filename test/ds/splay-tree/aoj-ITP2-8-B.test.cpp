#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/courses/lesson/8/ITP2/all/ITP2_8_B"
#include <iostream>
#include <string>

#include "nono/ds/splay-tree.hpp"

namespace nono {

void solve() {
    SplayTree<std::string, int> splay;
    int q;
    std::cin >> q;
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            std::string key;
            int x;
            std::cin >> key >> x;
            if (splay.contains(key)) {
                splay.get(key) = x;
            } else {
                splay.insert(key, x);
            }
        } else if (t == 1) {
            std::string key;
            std::cin >> key;
            if (splay.contains(key)) {
                std::cout << splay.get(key) << std::endl;
            } else {
                std::cout << 0 << std::endl;
            }
        } else {
            std::string key;
            std::cin >> key;
            if (splay.contains(key)) {
                splay.erase(key);
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
