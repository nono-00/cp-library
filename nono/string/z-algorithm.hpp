#pragma once

#include <vector>
#include <ranges>

namespace nono {

template <std::ranges::random_access_range R>
std::vector<int> z_algorithm(const R& sequence) {
    int n = std::size(sequence);
    std::vector<int> z_values(n, n);
    // [left, right)
    int right = 0;
    int left = 0;

    for (int i = 1; i < n; i++) {
        if (right <= i) {
            left = i;
            right = i;
            while (right < n && sequence[right - left] == sequence[right]) {
                right++;
            }
            z_values[i] = right - left;
        } else if (z_values[i - left] < right - i) {
            z_values[i] = z_values[i - left];
        } else {
            left = i;
            while (right < n && sequence[right - left] == sequence[right]) {
                right++;
            }
            z_values[i] = right - i;
        }
    }

    return z_values;
}

}  //  namespace nono
