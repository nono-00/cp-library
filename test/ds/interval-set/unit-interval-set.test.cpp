// competitive-verifier: STANDALONE
#include <cassert>
#include <random>
#include <set>

#include "nono/ds/interval-set.hpp"

namespace nono {

void solve() {
    constexpr int N = 60;
    std::mt19937 mt(1);
    std::uniform_int_distribution<int> pos(0, N - 1);

    IntervalSet<int> iset;
    std::set<int> naive;

    auto check = [&] {
        for (int i = 0; i < N; i++) {
            assert(iset.contains(i) == naive.contains(i));
        }
        assert((int)iset.size() == (int)naive.size());
        assert(iset.empty() == naive.empty());
    };
    check();

    for (int iter = 0; iter < 300; iter++) {
        int l = pos(mt), r = pos(mt);
        if (l > r) std::swap(l, r);
        r++;
        if (iter % 2 == 0) {
            iset.add(l, r);
            for (int i = l; i < r; i++) naive.insert(i);
        } else {
            iset.erase(l, r);
            for (int i = l; i < r; i++) naive.erase(i);
        }
        check();
    }

    iset.clear();
    naive.clear();
    check();

    //  begin/end で区間を列挙できる
    IntervalSet<int> small;
    small.add(0, 3);
    small.add(5, 8);
    int cnt = 0;
    for (auto it = small.begin(); it != small.end(); ++it) cnt++;
    assert(cnt == 2);
}

}  //  namespace nono

int main() {
    nono::solve();
}
