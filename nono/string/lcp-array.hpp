#pragma once

#include <string>
#include <vector>

#include "string/suffix-array.hpp"

namespace nono {

//  brief:
//  - Longest Common Prefix Array
//
//  details:
//  - LCPArray[i]: SuffixArray[i], SuffixArray[i + 1]に共通する
//  - 最大のprefixの長さ
//
//  depend:
//  - Suffix Array
//
//  see:
//  - 蟻本p.339
class LCPArray: public std::vector<int> {
  public:
    //  brief:
    //  - コンストラクタ
    //
    //  complexity:
    //  - O(N)
    //  - Suffix Arrayの構築込みだとO(N (logN) ^ 2)
    LCPArray() = default;
    LCPArray(const std::string& str) {
        this->operator=(LCPArray(str, SuffixArray(str)));
    }
    LCPArray(const std::string& str, const SuffixArray& sa) {
        int n = str.size();
        std::vector<int> data(n);
        std::vector<int> rank(n + 1);
        for (int i = 0; i <= n; i++) {
            rank[sa[i]] = i;
        }

        data[0] = 0;
        int h = 0;
        for (int i = 0; i < n; i++) {
            int j = sa[rank[i] - 1];
            if (h > 0) h--;
            for (; i + h < n && j + h < n; h++) {
                if (str[i + h] != str[j + h]) break;
            }
            data[rank[i] - 1] = h;
        }
        this->std::vector<int>::operator=(std::move(data));
    }
};

}  //  namespace nono
