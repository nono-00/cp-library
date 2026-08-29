// competitive-verifier: STANDALONE
#include <cassert>
#include <deque>
#include <random>

#include "nono/ds/two-stack-deque.hpp"
#include "nono/structure/monoid/add.hpp"

namespace nono {

using Deque = nono::TwoStackDeque<monoid::Add<int>>;

void check(Deque& deque, const std::deque<int>& reference) {
    assert((int)reference.size() == deque.size());
    assert(reference.empty() == deque.empty());
    if (!reference.empty()) {
        assert(deque.front() == reference.front());
        assert(deque.back() == reference.back());
    }
}

void solve() {
    std::mt19937 rng(20260820);
    Deque deque;
    std::deque<int> reference;

    for (int i = 0; i < 200; i++) {
        int op = rng() % 4;
        int v = (int)(rng() % 1000);
        if (op == 0) {
            deque.push_front(v);
            reference.push_front(v);
        } else if (op == 1) {
            deque.push_back(v);
            reference.push_back(v);
        } else if (op == 2 && !reference.empty()) {
            deque.pop_front();
            reference.pop_front();
        } else if (op == 3 && !reference.empty()) {
            deque.pop_back();
            reference.pop_back();
        }
        check(deque, reference);
    }

    //  片側に積んでからもう片側を触ることで内部リバランスを強制する
    for (int i = 0; i < 20; i++) {
        deque.push_back(i);
        reference.push_back(i);
    }
    while (!reference.empty()) {
        deque.pop_front();
        reference.pop_front();
        check(deque, reference);
    }
    for (int i = 0; i < 20; i++) {
        deque.push_front(i);
        reference.push_front(i);
    }
    while (!reference.empty()) {
        deque.pop_back();
        reference.pop_back();
        check(deque, reference);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
