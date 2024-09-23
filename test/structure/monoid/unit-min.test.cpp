#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <optional>

#include "nono/structure/monoid.hpp"
#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Min<unsigned>;
    //  fix
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            for (unsigned k = 0; k < 3; k++) {
                assert(test_associativity<Monoid>(i, j, k));
            }
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            assert(test_associativity<Monoid>(i, j, std::nullopt));
            assert(test_associativity<Monoid>(i, std::nullopt, j));
            assert(test_associativity<Monoid>(std::nullopt, i, j));
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        assert(test_associativity<Monoid>(i, std::nullopt, std::nullopt));
        assert(test_associativity<Monoid>(std::nullopt, i, std::nullopt));
        assert(test_associativity<Monoid>(std::nullopt, std::nullopt, i));
    }
    for (unsigned i = 0; i < 10; i++) {
        assert(test_identity<Monoid>(i));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
