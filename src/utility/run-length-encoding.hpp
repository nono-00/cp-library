#pragma once

#include <utility>
#include <ranges>
#include <vector>

namespace nono {

template <std::ranges::contiguous_range R>
std::vector<std::pair<typename R::value_type, int>> run_length_encoding(const R& sequence) {
    if (std::ranges::empty(sequence)) return {};
    using T = typename R::value_type;
    T prev = *std::ranges::begin(sequence);
    int count = 0;
    std::vector<std::pair<T, int>> result;
    for (auto elem: sequence) {
        if (elem != prev) {
            result.emplace_back(prev, count);
            prev = elem;
            count = 0;
        }
        count++;
    }
    result.emplace_back(prev, count);
    return result;
}

}  //  namespace nono
