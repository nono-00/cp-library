#define PROBLEM "https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ITP1_1_A"
#include <cassert>
#include <iostream>

#include "nono/structure/monoid.hpp"

using Monoid = nono::monoid::Rev<nono::monoid::Add<unsigned>>;
using Value = Monoid::Value;

bool operator==(Value lhs, Value rhs) {
    return lhs.ord == rhs.ord && lhs.rev == rhs.rev;
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
}

}  //  namespace nono

int main() {
    nono::solve();
    std::cout << "Hello World" << std::endl;
}
