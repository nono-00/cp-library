// competitive-verifier: STANDALONE
#include <cassert>
#include <string>
#include <vector>

#include "nono/utility/run-length-encoding.hpp"

namespace nono {

void solve() {
    //  文字列
    {
        auto rle = run_length_encoding(std::string("aababbba"));
        assert(rle.size() == 5);
        assert(rle[0].value == 'a' && rle[0].count == 2);
        assert(rle[1].value == 'b' && rle[1].count == 1);
        assert(rle[2].value == 'a' && rle[2].count == 1);
        assert(rle[3].value == 'b' && rle[3].count == 3);
        assert(rle[4].value == 'a' && rle[4].count == 1);
    }

    //  空
    {
        auto rle = run_length_encoding(std::string(""));
        assert(rle.empty());
    }

    //  全て同じ要素
    {
        auto rle = run_length_encoding(std::vector<int>{5, 5, 5, 5});
        assert(rle.size() == 1);
        assert(rle[0].value == 5 && rle[0].count == 4);
    }

    //  全て異なる要素
    {
        auto rle = run_length_encoding(std::vector<int>{1, 2, 3});
        assert(rle.size() == 3);
        for (int i = 0; i < 3; i++) {
            assert(rle[i].value == i + 1 && rle[i].count == 1);
        }
    }

    //  復元できることを確認
    {
        std::vector<int> original = {7, 7, 3, 3, 3, 9, 9, 1};
        auto rle = run_length_encoding(original);
        std::vector<int> restored;
        for (auto& p: rle) {
            for (int i = 0; i < p.count; i++) restored.push_back(p.value);
        }
        assert(restored == original);
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
