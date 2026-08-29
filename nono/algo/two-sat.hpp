#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "../graph/base.hpp"
#include "../graph/strongly-connected-components.hpp"

namespace nono {

///  # 2sat
///  (A_1 or A_2) and (A_3 or A_4) ... が真となる割り当てが存在するか
class TwoSat {
  public:
    TwoSat() {}
    TwoSat(int n): n_(n), ans_(n) {}

    ///  # add_clause(i, j)
    ///  (i or j)を追加する
    ///  肯定リテラル  i
    ///  否定リテラル ~i
    ///  iを恒真にしたい => add_clause(i, i)
    ///  O(1)
    void add_clause(int i, int j) {
        assert((0 <= i && i < n_) || (0 <= ~i && ~i < n_));
        assert((0 <= j && j < n_) || (0 <= ~j && ~j < n_));
        edges_.emplace_back((i < 0 ? 2 * ~i : 2 * i + 1), (j < 0 ? 2 * ~j + 1 : 2 * j));
        edges_.emplace_back((j < 0 ? 2 * ~j : 2 * j + 1), (i < 0 ? 2 * ~i + 1 : 2 * i));
    }

    ///  # satisfiable()
    ///  条件を満たす割り当てが存在するか
    ///  O(N + M)
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

    ///  # answer()
    ///  実際の割り当てを返す
    ///  存在しない場合、壊れる
    std::vector<bool> answer() {
        bool exist = satisfiable();
        assert(exist);
        return ans_;
    }

  private:
    int n_;
    std::vector<bool> ans_;
    std::vector<Edge> edges_;
};

}  //  namespace nono
