#pragma once

#include <ranges>
#include <vector>

namespace nono {

namespace internal {

template <class T>
struct RLEPair {
    T value;
    int count;
};

}  //  namespace internal

///  # run length encoding
///  aababbba => [(a, 2), (b, 1), (a, 1), (b, 3), (a, 1)]
///  O(|seq|)
template <std::ranges::random_access_range R>
std::vector<internal::RLEPair<typename R::value_type>> run_length_encoding(const R& sequence) {
    if (std::ranges::empty(sequence)) return {};
    std::vector<internal::RLEPair<typename R::value_type>> result;
    for (auto lit = std::ranges::begin(sequence); lit != std::ranges::end(sequence);) {
        auto rit = lit;
        while (rit != std::ranges::end(sequence) && *lit == *rit) rit++;
        result.emplace_back(*lit, rit - lit);
        lit = rit;
    }
    return result;
}

}  //  namespace nono
