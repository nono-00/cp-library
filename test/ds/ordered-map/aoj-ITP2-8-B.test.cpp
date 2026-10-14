// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/courses/lesson/8/ITP2/all/ITP2_8_B
#include <iostream>
#include <string>

#include "nono/ds/ordered-map.hpp"

namespace nono {

void solve() {
    OrderedMap<std::string, int> map;
    int q;
    std::cin >> q;
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            std::string key;
            int x;
            std::cin >> key >> x;
            map.set(key, x);
        } else if (t == 1) {
            std::string key;
            std::cin >> key;
            if (map.contains(key)) {
                std::cout << map.get(key) << std::endl;
            } else {
                std::cout << 0 << std::endl;
            }
        } else {
            std::string key;
            std::cin >> key;
            if (map.contains(key)) {
                map.erase(key);
            }
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
