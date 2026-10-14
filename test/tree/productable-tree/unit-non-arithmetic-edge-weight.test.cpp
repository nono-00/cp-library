// competitive-verifier: STANDALONE
#include <cassert>
#include <vector>

#include "nono/structure/act-monoid/act-monoid.hpp"
#include "nono/structure/monoid/add.hpp"
#include "nono/tree/productable-tree.hpp"

namespace nono {

//  operator+を持たない辺重み型でもProductableTreeが構築できることを確認する回帰テスト
//  (issue #13: 辺重みTにoperator+を要求してしまっていた問題)
struct NoAddWeight {
    int label = 0;
};

void solve() {
    using Monoid = act_monoid::ActMonoid<monoid::Add<long long>>;

    std::vector<EdgeBase<NoAddWeight>> edges = {
        {0, 1, NoAddWeight{1}},
        {1, 2, NoAddWeight{2}},
        {1, 3, NoAddWeight{3}},
    };
    std::vector<long long> value = {10, 20, 30, 40};

    ProductableTree<Monoid, NoAddWeight> tree(4, edges, value);

    assert(tree.prod_path(0, 2) == 60);
    assert(tree.prod_path(2, 3) == 90);
    assert(tree.prod_subtree(1) == 90);

    tree.set(0, value[0] += 5);
    assert(tree.prod_path(0, 2) == 65);
}

}  //  namespace nono

int main() {
    nono::solve();
}
