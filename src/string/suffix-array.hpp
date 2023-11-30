#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

namespace nono {

//  brief:
//  - Suffix Array 接尾辞配列
class SuffixArray: public std::vector<int> {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(N (logN) ^ 2)
    //
    //  note:
    //  - Manber & Myers algorithm
    //  - SA-ISはO(N)なのでもっと速い
    //
    //  see:
    //  - 蟻本 p337
    SuffixArray() = default;
    SuffixArray(const std::string& str) {
        int n = str.size();
        std::vector<int> data(n + 1);
        std::vector<int> rank(n + 1, -1);
        std::vector<int> temp(n + 1);
        std::iota(data.begin(), data.end(), 0);
        for (int i = 0; i < n; i++) {
            rank[i] = (int)(str[i]);
        }

        for (int k = 1; k <= n; k *= 2) {
            auto comp = [&](int lhs, int rhs) {
                if (rank[lhs] != rank[rhs]) return rank[lhs] < rank[rhs];
                if (lhs + k > n || rhs + k > n) return (lhs + k > n);
                return rank[lhs + k] < rank[rhs + k];
            };
            std::sort(data.begin(), data.end(), comp);
            temp[data[0]] = 0;
            for (int i = 0; i < n; i++) {
                temp[data[i + 1]] = temp[data[i]] + (comp(data[i], data[i + 1]) ? 1 : 0);
            }
            rank.assign(temp.begin(), temp.end());
        }
        this->std::vector<int>::operator=(std::move(data));
    }
};

}  //  namespace nono
