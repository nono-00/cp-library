// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <random>
#include <utility>
#include <vector>

#include "nono/ds/sequence.hpp"

namespace nono {

void check(Sequence<int>& seq, const std::vector<int>& reference) {
    assert(seq.size() == (int)reference.size());
    assert(seq.empty() == reference.empty());
    for (int i = 0; i < (int)reference.size(); i++) {
        assert(seq.get(i) == reference[i]);
        assert(seq[i] == reference[i]);
        assert(std::as_const(seq)[i] == reference[i]);
    }
}

void test_basic_operations() {
    std::mt19937 rng(20260821);
    Sequence<int> seq;
    std::vector<int> reference;

    for (int i = 0; i < 50; i++) {
        int v = (int)(rng() % 1000);
        int k = (int)(rng() % (reference.size() + 1));
        seq.insert(k, v);
        reference.insert(reference.begin() + k, v);
        check(seq, reference);
    }

    for (int i = 0; i < 500; i++) {
        int op = (int)(rng() % 9);
        if (op == 0 && !reference.empty()) {
            int k = (int)(rng() % reference.size());
            int v = (int)(rng() % 1000);
            seq.insert(k, v);
            reference.insert(reference.begin() + k, v);
        } else if (op == 1 && !reference.empty()) {
            int k = (int)(rng() % reference.size());
            seq.erase(k);
            reference.erase(reference.begin() + k);
        } else if (op == 2) {
            int v = (int)(rng() % 1000);
            seq.push_back(v);
            reference.push_back(v);
        } else if (op == 3 && !reference.empty()) {
            seq.pop_back();
            reference.pop_back();
        } else if (op == 4) {
            int v = (int)(rng() % 1000);
            seq.push_front(v);
            reference.insert(reference.begin(), v);
        } else if (op == 5 && !reference.empty()) {
            seq.pop_front();
            reference.erase(reference.begin());
        } else if (op == 6 && !reference.empty()) {
            int k = (int)(rng() % reference.size());
            int v = (int)(rng() % 1000);
            seq.set(k, v);
            reference[k] = v;
        } else if (op == 7 && !reference.empty()) {
            int k = (int)(rng() % reference.size());
            int v = (int)(rng() % 1000);
            seq[k] = v;
            reference[k] = v;
        } else if (op == 8 && !reference.empty()) {
            int l = (int)(rng() % reference.size());
            int r = l + (int)(rng() % (reference.size() - l + 1));
            seq.reverse(l, r);
            std::reverse(reference.begin() + l, reference.begin() + r);
        }
        check(seq, reference);
    }

    while (!reference.empty()) {
        seq.pop_back();
        reference.pop_back();
        check(seq, reference);
    }

    seq.clear();
    assert(seq.empty());
    assert(seq.size() == 0);
}

void test_split_merge() {
    std::mt19937 rng(20260822);
    for (int iter = 0; iter < 30; iter++) {
        int n = (int)(rng() % 50);
        Sequence<int> seq;
        std::vector<int> reference;
        for (int i = 0; i < n; i++) {
            int v = (int)(rng() % 1000);
            seq.push_back(v);
            reference.push_back(v);
        }
        int k = (int)(rng() % (n + 1));
        auto [lhs, rhs] = seq.split(k);
        std::vector<int> lhs_ref(reference.begin(), reference.begin() + k);
        std::vector<int> rhs_ref(reference.begin() + k, reference.end());
        check(lhs, lhs_ref);
        check(rhs, rhs_ref);

        Sequence<int> merged = merge(lhs, rhs);
        check(merged, reference);
    }
}

void test_copy_move() {
    std::mt19937 rng(20260823);
    Sequence<int> seq;
    std::vector<int> reference;
    for (int i = 0; i < 30; i++) {
        int v = (int)(rng() % 1000);
        seq.push_back(v);
        reference.push_back(v);
    }

    Sequence<int> copy_seq(seq);
    check(copy_seq, reference);
    copy_seq.push_back(9999);
    check(seq, reference);
    std::vector<int> copy_ref = reference;
    copy_ref.push_back(9999);
    check(copy_seq, copy_ref);

    Sequence<int> assigned;
    assigned = seq;
    check(assigned, reference);
    assigned.set(0, -1);
    check(seq, reference);
    std::vector<int> assigned_ref = reference;
    assigned_ref[0] = -1;
    check(assigned, assigned_ref);

    Sequence<int> moved(std::move(assigned));
    check(moved, assigned_ref);
    assert(assigned.size() == 0);

    Sequence<int> move_assigned;
    move_assigned = std::move(moved);
    check(move_assigned, assigned_ref);
    assert(moved.size() == 0);

    move_assigned.clear();
    assert(move_assigned.empty());
}

void solve() {
    test_basic_operations();
    test_split_merge();
    test_copy_move();
}

}  //  namespace nono

int main() {
    nono::solve();
}
