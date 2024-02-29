#define IGNORE
#define PROBLEM "https://atcoder.jp/contests/arc033/tasks/arc033_3"

#include <iostream>

#include "nono/data-structure/binary-trie.hpp"

namespace nono {

void solve() {
    int q;
    std::cin >> q;
    BinaryTrie trie;
    while (q--) {
        int t;
        unsigned x;
        std::cin >> t >> x;
        if (t == 1) {
            trie.insert(x);
        } else {
            x--;
            auto ans = trie.kth_element(x);
            std::cout << ans << '\n';
            trie.erase(ans);
        }
    }
}

}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
