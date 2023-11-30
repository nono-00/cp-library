#pragma once

#include <ranges>
#include <string>
#include <vector>

namespace nono {

//  brief:
//  - manacher
//  - 列に対して、ある文字を中心とする回文の最大半径を求める
//
//  complexity:
//  - O(|S|)
//
//  note:
//  - 偶数長の文字列を求めたい場合は
//  - 適当な文字を間に挟む
template <std::ranges::random_access_range R>
std::vector<int> manacher(const R& sequence) {
    int size = std::size(sequence);
    std::vector<int> result(size);

    int i = 0;
    int j = 1;
    while (i < size) {
        while (i + j < size && i - j >= 0 && sequence[i + j] == sequence[i - j]) j++;
        result[i] = j;

        int k = 1;
        while (i + k < size && i - k >= 0 && k + result[i - k] < result[i]) {
            result[i + k] = result[i - k];
            k++;
        }
        i += k;
        j -= k;
    }

    return result;
}

}  //  namespace nono
