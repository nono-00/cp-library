#define PROBLEM "https://judge.yosupo.jp/problem/deque_operate_all_composite"
#include <iostream>

#include "../../../nono/ds/two-stack-deque.hpp"
#include "../../../nono/math/modint.hpp"
#include "../../../nono/structure/monoid.hpp"

namespace nono {

void solve() {
    using Mint = Modint998244353;
    using Monoid = monoid::Composite<Mint>;
    using Value = Monoid::Value;
    int q;
    std::cin >> q;
    TwoStackDeque<Monoid> que;
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            Mint a, b;
            std::cin >> a >> b;
            que.push_front(Value{a, b});
        } else if (t == 1) {
            Mint a, b;
            std::cin >> a >> b;
            que.push_back(Value{a, b});
        } else if (t == 2) {
            que.pop_front();
        } else if (t == 3) {
            que.pop_back();
        } else {
            Mint x;
            std::cin >> x;
            std::cout << que.prod().eval(x) << '\n';
        }
    }
}

}  //  namespace nono

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}
