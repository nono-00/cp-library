// competitive-verifier: STANDALONE
#include <cassert>

#include "nono/structure/monoid/min-index.hpp"

using Monoid = nono::monoid::MinIndex<unsigned>;
using Value = Monoid::Value;

bool operator==(Value lhs, Value rhs) {
    return lhs.value == rhs.value && lhs.index == rhs.index;
}

#include "nono/structure/law-checker.hpp"

namespace nono {

void solve() {
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            for (unsigned k = 0; k < 3; k++) {
                assert(test_value_associativity<Monoid>(Value{i, 0}, Value{j, 1}, Value{k, 2}));
            }
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            assert(test_value_associativity<Monoid>(Value{i, 0}, Value{j, 1}, Monoid::e()));
            assert(test_value_associativity<Monoid>(Value{i, 0}, Monoid::e(), Value{j, 2}));
            assert(test_value_associativity<Monoid>(Monoid::e(), Value{i, 1}, Value{j, 2}));
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        assert(test_value_associativity<Monoid>(Value{i, 0}, Monoid::e(), Monoid::e()));
        assert(test_value_associativity<Monoid>(Monoid::e(), Value{i, 1}, Monoid::e()));
        assert(test_value_associativity<Monoid>(Monoid::e(), Monoid::e(), Value{i, 2}));
    }
    for (unsigned i = 0; i < 10; i++) {
        assert(test_value_identity<Monoid>(Value{i, 0}));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
