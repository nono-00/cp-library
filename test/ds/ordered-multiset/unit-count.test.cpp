// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include "nono/ds/ordered-multiset.hpp"

namespace nono {

///  OrderedMultiSet::count(key)が実際の出現回数を正しく返すことを確認する回帰テスト
///  (count()がsuccessorのrankをそのまま返してしまい、
///   query key未満の要素数を引き忘れていたバグの修正)
void solve() {
    std::mt19937 rng(20260821);
    OrderedMultiSet<int> set;
    std::vector<int> reference;

    for (int i = 0; i < 200; i++) {
        int key = (int)(rng() % 20);
        set.insert(key);
        reference.push_back(key);

        for (int query = 0; query <= 20; query++) {
            int expected = (int)std::ranges::count(reference, query);
            assert(set.count(query) == expected);
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
