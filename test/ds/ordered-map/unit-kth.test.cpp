// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/ds/ordered-map.hpp"

namespace nono {

//  OrderedMap::kth(k)がKey/Value両方を正しく返すことを確認する回帰テスト
//  (kth()が誤ってKeyのみを返していたバグの修正)
void solve() {
    std::mt19937 rng(20260821);
    OrderedMap<int, int> map;
    std::vector<std::pair<int, int>> reference;

    for (int i = 0; i < 50; i++) {
        int key = (int)(rng() % 1000);
        int value = (int)(rng() % 1000);
        if (map.contains(key)) continue;
        map.set(key, value);
        reference.emplace_back(key, value);
        std::ranges::sort(reference);

        for (int k = 0; k < (int)reference.size(); k++) {
            auto [key_got, value_got] = map.kth(k);
            assert(key_got == reference[k].first);
            assert(value_got == reference[k].second);
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
