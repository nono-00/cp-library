#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <random>

#include "nono/structure/act-monoid.hpp"

using Monoid = nono::act_monoid::RangeAddCountMin<unsigned>;
using Value = Monoid::Value;
using Act = Monoid::Act;

bool operator==(Value lhs, Value rhs) {
    return lhs.min == rhs.min && lhs.num == rhs.num;
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
    assert(test_act_associativity<Monoid>(0, 0, 0));
    assert(test_act_associativity<Monoid>(10, 20, 30));
    assert(test_act_associativity<Monoid>(30, 50, 10));
    for (unsigned i = 0; i < 10; i++) {
        assert(test_act_identity<Monoid>(i));
    }
    assert(test_distributive<Monoid>(0, Value{0, 10}, Value{0, 0}));
    assert(test_distributive<Monoid>(10, Value{20, 10}, Value{30, 5}));
    assert(test_distributive<Monoid>(30, Value{20, 4}, Value{50, 10}));

    //  random
    const int lim = 1000;
    std::mt19937 rng(1459);
    const int T = 10;
    for (int _ = 0; _ < T; _++) {
        Value value[3] = {{rng() % lim, rng() % lim}, {rng() % lim, rng() % lim}, {rng() % lim, rng() % lim}};
        Act act[3] = {rng() % lim, rng() % lim, rng() % lim};
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
