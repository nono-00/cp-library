// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <vector>

#include "nono/ds/csr-array.hpp"

namespace nono {

void solve() {
    //  (key, value)ペアのコンストラクタ
    {
        std::vector<std::pair<int, int>> data = {{0, 10}, {2, 20}, {0, 11}, {1, 30}, {2, 21}};
        CSRArray<int> csr(3, data);
        assert(csr.size() == 3);
        assert(csr.all_size() == 5);

        std::vector<int> row0(csr[0].begin(), csr[0].end());
        std::vector<int> row1(csr[1].begin(), csr[1].end());
        std::vector<int> row2(csr[2].begin(), csr[2].end());
        std::sort(row0.begin(), row0.end());
        std::sort(row2.begin(), row2.end());
        assert((row0 == std::vector<int>{10, 11}));
        assert((row1 == std::vector<int>{30}));
        assert((row2 == std::vector<int>{20, 21}));
    }

    //  row配列 + data配列のコンストラクタ
    {
        std::vector<int> row = {1, 0, 1, 2};
        std::vector<int> data = {100, 200, 101, 300};
        const CSRArray<int> csr(3, row, data);
        assert(csr.size() == 3);
        assert(csr.all_size() == 4);

        std::vector<int> row0(csr[0].begin(), csr[0].end());
        std::vector<int> row1(csr[1].begin(), csr[1].end());
        std::vector<int> row2(csr[2].begin(), csr[2].end());
        std::sort(row0.begin(), row0.end());
        std::sort(row1.begin(), row1.end());
        assert((row0 == std::vector<int>{200}));
        assert((row1 == std::vector<int>{100, 101}));
        assert((row2 == std::vector<int>{300}));
    }

    //  空の行を持つケース
    {
        std::vector<std::pair<int, int>> data = {{2, 5}};
        CSRArray<int> csr(3, data);
        assert(csr[0].begin() == csr[0].end());
        assert(csr[1].begin() == csr[1].end());
        assert(std::vector<int>(csr[2].begin(), csr[2].end()) == std::vector<int>{5});
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
