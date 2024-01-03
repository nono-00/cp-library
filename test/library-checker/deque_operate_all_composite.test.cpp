#define PROBLEM "https://judge.yosupo.jp/problem/deque_operate_all_composite"
#include <iostream>
#include <iomanip>

#include "data-structure/two-stack-deque.hpp"
#include "math/modint.hpp"

namespace nono {

using Mint = nono::Modint<998244353>;

struct Data {
    Mint coef;
    Mint constant;
    Data(Mint coef, Mint constant): coef(coef), constant(constant) {}
    Mint eval(Mint x) const {
        return coef * x + constant;
    }
};

Data e() {
    return Data(1, 0);
}

Data op(Data lhs, Data rhs) {
    return Data(lhs.coef * rhs.coef, lhs.coef * rhs.constant + lhs.constant);
}

void solve() {
    int q;
    std::cin >> q;
    nono::TwoStackDeque<Data, op, e> que;
    while (q--) {
        int t;
        std::cin >> t;
        if (t == 0) {
            Mint a, b;
            std::cin >> a >> b;
            que.push_front(Data(a, b));
        } else if (t == 1) {
            Mint a, b;
            std::cin >> a >> b;
            que.push_back(Data(a, b));
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

}

int main() {
    std::cin.tie(0)->sync_with_stdio(0);
    nono::solve();
}

