#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <random>

#include "nono/math/modint.hpp"
#include "nono/structure/monoid.hpp"

using Mint = nono::Modint998244353;
using Monoid = nono::monoid::Composite<Mint>;
using Value = Monoid::Value;
bool operator==(Value lhs, Value rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b;
}

#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    //  fix
    assert(test_value_associativity<Monoid>(Value{1, 0}, Value{0, 0}, Value{10, 20}));
    assert(test_value_associativity<Monoid>(Value{10, 20}, Value{20, 50}, Value{30, 1}));
    assert(test_value_associativity<Monoid>(Value{30, 10}, Value{30, 40}, Value{50, 0}));
    assert(test_value_identity<Monoid>(Value{30, 10}));
    assert(test_value_identity<Monoid>(Value{30, 40}));
    assert(test_value_identity<Monoid>(Value{50, 0}));

    //  random
    std::mt19937 rng(1119);
    const int T = 10;
    for (int i = 0; i < T; i++) {
        Value a{Mint{rng()}, Mint{rng()}};
        Value b{Mint{rng()}, Mint{rng()}};
        Value c{Mint{rng()}, Mint{rng()}};
        assert(test_value_associativity<Monoid>(a, b, c));
        assert(test_value_associativity<Monoid>(a, c, b));
        assert(test_value_associativity<Monoid>(b, a, c));
        assert(test_value_associativity<Monoid>(b, c, a));
        assert(test_value_associativity<Monoid>(c, a, b));
        assert(test_value_associativity<Monoid>(c, b, a));
        assert(test_value_identity<Monoid>(a));
        assert(test_value_identity<Monoid>(b));
        assert(test_value_identity<Monoid>(c));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
