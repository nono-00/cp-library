// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>

#include "nono/structure/monoid/add.hpp"
#include "nono/structure/law-checker.hpp"

namespace nono {

void solve() {
    using Monoid = monoid::Add<unsigned>;
    //  fix
    assert(test_value_associativity<Monoid>(0, 0, 0));
    assert(test_value_associativity<Monoid>(10, 20, 30));
    assert(test_value_associativity<Monoid>(30, 20, 50));
    for (unsigned i = 0; i < 10; i++) {
        assert(test_value_identity<Monoid>(i));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}

