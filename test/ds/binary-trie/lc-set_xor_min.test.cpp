#define PROBLEM "https://judge.yosupo.jp/problem/set_xor_min"

#include <cassert>
#include <iostream>
#include <random>

#include "nono/ds/binary-trie.hpp"

namespace nono {

void solve() {
    std::mt19937 mt(std::random_device{}());
    int q;
    std::cin >> q;
    BinaryTrie trie;
    while (q--) {
        int op;
        unsigned x;
        std::cin >> op >> x;
        if (op == 0) {
            if (!trie.contains(x)) {
                assert(trie.count(x) == 0);
                trie.insert(x);
                assert(trie.count(x) == 1);
                assert(trie.contains(x));
            } else {
                assert(trie.count(x) == 1);
            }
        } else if (op == 1) {
            if (trie.contains(x)) {
                assert(trie.count(x) == 1);
                trie.erase(x);
                assert(trie.count(x) == 0);
                assert(!trie.contains(x));
            } else {
                assert(trie.count(x) == 0);
            }
        } else {
            assert(!trie.empty());
            unsigned values[2];
            values[0] = trie.min(x);
            trie.apply_xor(x);
            values[1] = trie.min();
            trie.apply_xor(x);
            assert(values[0] == values[1]);
            std::cout << values[0] << '\n';
        }
        if (!trie.empty()) {
            int k = mt() % (unsigned)trie.size();
            auto value = trie.kth(k);
            assert(trie.count_lt(value) == k);
            assert(k + trie.count_gt(value) + 1 == trie.size());
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
