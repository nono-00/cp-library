#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <random>

#include "nono/math/modint.hpp"
#include "nono/structure/act-monoid.hpp"

using Mint = nono::Modint998244353;
using Monoid = nono::act_monoid::RangeAffineRangeSum<Mint>;
using Value = Monoid::Value;
using Act = Monoid::Act;

bool operator==(Value lhs, Value rhs) {
    return lhs.sum == rhs.sum && lhs.num == rhs.num;
}

bool operator==(Act lhs, Act rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b;
}

#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    //  fix
    assert(test_value_associativity<Monoid>(Value{0, 1}, Value{0, 10}, Value{0, 0}));
    assert(test_value_associativity<Monoid>(Value{10, 2}, Value{20, 10}, Value{30, 5}));
    assert(test_value_associativity<Monoid>(Value{30, 2}, Value{20, 4}, Value{50, 10}));
    for (unsigned i = 0; i < 10; i++) {
        assert(test_value_identity<Monoid>(Value{i, 1}));
    }
    assert(test_act_associativity<Monoid>(Act{0, 1}, Act{0, 10}, Act{0, 0}));
    assert(test_act_associativity<Monoid>(Act{10, 2}, Act{20, 10}, Act{30, 5}));
    assert(test_act_associativity<Monoid>(Act{30, 2}, Act{20, 4}, Act{50, 10}));
    for (unsigned i = 0; i < 10; i++) {
        assert(test_act_identity<Monoid>(Act{i, 1}));
    }
    assert(test_distributive<Monoid>(Act{0, 1}, Value{0, 10}, Value{0, 0}));
    assert(test_distributive<Monoid>(Act{10, 2}, Value{20, 10}, Value{30, 5}));
    assert(test_distributive<Monoid>(Act{30, 2}, Value{20, 4}, Value{50, 10}));

    //  random
    const int lim = 1000;
    std::mt19937 rng(1451);
    const int T = 10;
    for (int _ = 0; _ < T; _++) {
        Value value[3] = {{rng(), rng() % lim}, {rng(), rng() % lim}, {rng(), rng() % lim}};
        Act act[3] = {{rng(), rng()}, {rng(), rng()}, {rng(), rng()}};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    assert(test_value_associativity<Monoid>(value[i], value[j], value[k]));
                    assert(test_act_associativity<Monoid>(act[i], act[j], act[k]));
                    assert(test_distributive<Monoid>(act[i], value[j], value[k]));
                }
            }
        }
        for (int i = 0; i < 3; i++) {
            assert(test_value_identity<Monoid>(value[i]));
            assert(test_act_identity<Monoid>(act[i]));
        }
    }
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
