#define PROBLEM "https://judge.yosupo.jp/problem/set_xor_min"

#include <cassert>
#include <iostream>

#include "nono/ds/binary-trie.hpp"

namespace nono {

void solve() {
    int q;
    std::cin >> q;
    BinaryTrie32 trie;
    while (q--) {
        int op;
        unsigned x;
        std::cin >> op >> x;
        if (op == 0) {
            if (!trie.contains(x)) {
                trie.insert(x);
            }
        } else if (op == 1) {
            if (trie.contains(x)) {
                trie.erase(x);
            }
        } else {
            auto lhs = x ^ trie.min_element(x);
            trie.all_xor(x);
            auto rhs = trie.min_element();
            trie.all_xor(x);
            assert(lhs == rhs);
            std::cout << rhs << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
