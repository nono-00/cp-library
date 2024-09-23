#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <random>

#include "nono/structure/monoid.hpp"
#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Gcd<unsigned>;
    //  fix
    assert(test_associativity<Monoid>(0, 0, 1));
    assert(test_associativity<Monoid>(1, 10, 30));
    assert(test_associativity<Monoid>(2, 3, 5));
    assert(test_associativity<Monoid>(10, 10, 10));
    assert(test_associativity<Monoid>(100, 10, 50));

    //  random
    std::mt19937 rng(1134);
    const int T = 10;
    for (int i = 0; i < T; i++) {
        unsigned a = rng();
        unsigned b = rng();
        unsigned c = rng();
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
