#pragma once

#include <ranges>
#include <vector>

namespace nono {

namespace run_length_encoding_internal {

template <class T>
struct RLEPair {
    T value;
    int count;
};

}  //  namespace run_length_encoding_internal

//  # run_length_encoding(sequence)
//  ランレングス圧縮。同じ値が連続する区間ごとに (value, count) へまとめた列を返す
//  aababbba => [(a, 2), (b, 1), (a, 1), (b, 3), (a, 1)]
//  O(|sequence|)
template <std::ranges::random_access_range R>
std::vector<run_length_encoding_internal::RLEPair<typename R::value_type>> run_length_encoding(const R& sequence) {
    if (std::ranges::empty(sequence)) return {};
    std::vector<run_length_encoding_internal::RLEPair<typename R::value_type>> result;
    for (auto lit = std::ranges::begin(sequence); lit != std::ranges::end(sequence);) {
        auto rit = lit;
        while (rit != std::ranges::end(sequence) && *lit == *rit) rit++;
        result.emplace_back(*lit, rit - lit);
        lit = rit;
    }
    return result;
}

}  //  namespace nono
