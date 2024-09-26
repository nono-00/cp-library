#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "nono/graph/base.hpp"
#include "nono/graph/strongly-connected-components.hpp"

namespace nono {

///  brief : 2sat. \\( (x_1 \lor y_1) \land (x_2 \lor y_2) \land \dots \\) みたいなやつを解く.
class TwoSat {
  public:
    TwoSat() {}
    TwoSat(int n): n_(n), ans_(n) {}

    //  肯定リテラル  i
    //  否定リテラル ~i
    void add_clause(int i, int j) {
        assert((0 <= i && i < n_) || (0 <= ~i && ~i < n_));
        assert((0 <= j && j < n_) || (0 <= ~j && ~j < n_));
        edges_.emplace_back((i < 0 ? 2 * ~i : 2 * i + 1), (j < 0 ? 2 * ~j + 1 : 2 * j));
        edges_.emplace_back((j < 0 ? 2 * ~j : 2 * j + 1), (i < 0 ? 2 * ~i + 1 : 2 * i));
    }

    bool satisfiable() {
        auto groups = strongly_connected_components(to_directed_graph(2 * n_, edges_));
        std::vector<int> id(n_, -1);
        for (int i = 0; i < std::ssize(groups); i++) {
            for (auto v: groups[i]) {
                if (id[v / 2] == i) return false;
                if (id[v / 2] == -1) {
                    ans_[v / 2] = v & 1;
                    id[v / 2] = i;
                }
            }
        }
        return true;
    }

    std::vector<bool> answer() {
        return ans_;
    }

  private:
    int n_;
    std::vector<bool> ans_;
    std::vector<Edge> edges_;
};

}  //  namespace nono
