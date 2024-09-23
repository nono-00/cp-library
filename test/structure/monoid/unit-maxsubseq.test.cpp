#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <random>

#include "nono/structure/monoid.hpp"

using Monoid = nono::monoid::MaxSubSeq<unsigned>;
using Value = Monoid::Value;

bool operator==(Value lhs, Value rhs) {
    return lhs.val == rhs.val && lhs.prefix == rhs.prefix && lhs.suffix == rhs.suffix && lhs.sum == rhs.sum;
}

#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    //  fix
    assert(test_associativity<Monoid>(0, 0, 0));
    assert(test_associativity<Monoid>(10, 20, 30));
    assert(test_associativity<Monoid>(30, 20, 50));
    for (unsigned i = 0; i < 10; i++) {
        assert(test_identity<Monoid>(i));
    }

    //  random
    const unsigned lim = 100000;
    std::mt19937 rng(1142);
    const int T = 10;
    for (int i = 0; i < T; i++) {
        Value a(rng() % lim);
        Value b(rng() % lim);
        Value c(rng() % lim);
        assert(test_associativity<Monoid>(a, b, c));
        assert(test_associativity<Monoid>(a, c, b));
        assert(test_associativity<Monoid>(b, a, c));
        assert(test_associativity<Monoid>(b, c, a));
        assert(test_associativity<Monoid>(c, a, b));
        assert(test_associativity<Monoid>(c, b, a));
        assert(test_identity<Monoid>(a));
        assert(test_identity<Monoid>(b));
        assert(test_identity<Monoid>(c));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
