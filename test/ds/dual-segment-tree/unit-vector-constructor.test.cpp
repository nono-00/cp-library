// competitive-verifier: STANDALONE
#include <cassert>
#include <random>
#include <vector>

#include "nono/ds/dual-segment-tree.hpp"

namespace nono {

//  区間加算: x <- x + delta （DualSegmentTreeは可換な作用のみ対応のため加算を使う）
struct M {
    using Value = long long;
    using Act = long long;
    static Value mapping(Act f, Value v) {
        return v + f;
    }
    static Act composition(Act f, Act g) {
        return f + g;
    }
    static Act id() {
        return 0;
    }
    static Value e() {
        return 0;
    }
};

void check(DualSegmentTree<M>& segtree, const std::vector<long long>& reference) {
    for (int i = 0; i < (int)reference.size(); i++) {
        assert(segtree.get(i) == reference[i]);
    }
}

void solve() {
    std::mt19937 rng(20260820);
    const int n = 20;
    std::vector<long long> data(n);
    for (int i = 0; i < n; i++) data[i] = (long long)(rng() % 1000);

    //  ベクタ経由のコンストラクタで初期値が正しく保持されていることを確認
    DualSegmentTree<M> segtree(data);
    check(segtree, data);

    //  apply後もdata_(data_)のような未初期化状態に依存していないことを確認
    for (int t = 0; t < 20; t++) {
        int l = (int)(rng() % n);
        int r = l + 1 + (int)(rng() % (n - l));
        long long delta = (long long)(rng() % 1000) - 500;
        segtree.apply(l, r, delta);
        for (int i = l; i < r; i++) data[i] += delta;
        check(segtree, data);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
