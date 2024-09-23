#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>
#include <optional>

#include "nono/structure/monoid.hpp"

using Monoid = nono::monoid::MinIndex<unsigned>;
using Value = Monoid::Value;

bool operator==(Value lhs, Value rhs) {
    if (lhs && rhs) {
        return lhs->min == rhs->min && lhs->index == rhs->index;
    } else if (!lhs && !rhs) {
        return true;
    }
    return false;
}

#include "nono/structure/test.hpp"

namespace nono {

void solve() {
    //  fix
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            for (unsigned k = 0; k < 3; k++) {
                assert(test_value_associativity<Monoid>(Value{{i, 0}}, Value{{j, 1}}, Value{{k, 2}}));
            }
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        for (unsigned j = 0; j < 3; j++) {
            assert(test_value_associativity<Monoid>(Value{{i, 0}}, Value{{j, 1}}, std::nullopt));
            assert(test_value_associativity<Monoid>(Value{{i, 0}}, std::nullopt, Value{{j, 2}}));
            assert(test_value_associativity<Monoid>(std::nullopt, Value{{i, 1}}, Value{{j, 2}}));
        }
    }
    for (unsigned i = 0; i < 3; i++) {
        assert(test_value_associativity<Monoid>(Value{{i, 0}}, std::nullopt, std::nullopt));
        assert(test_value_associativity<Monoid>(std::nullopt, Value{{i, 1}}, std::nullopt));
        assert(test_value_associativity<Monoid>(std::nullopt, std::nullopt, Value{{i, 2}}));
    }
    for (unsigned i = 0; i < 10; i++) {
        assert(test_value_identity<Monoid>(Value{{i, 0}}));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
