#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

namespace nono {

//  # longest_increasing_subseq(seq, compare)
//  最長増加部分列(LIS)。隣り合う要素が compare(seq[LIS[i]], seq[LIS[i + 1]]) = true を満たす
//  最長の部分列を1つ求め、その添字の列を返す
//  compareが std::less なら狭義単調増加、std::less_equal なら広義単調増加、std::greater なら狭義単調減少
//  seqが空だと壊れる
//  O(n log n)
template <class T, class Compare>
std::vector<int> longest_increasing_subseq(const std::vector<T>& seq, Compare compare) {
    int n = seq.size();
    std::vector<int> parent(n);
    std::vector<int> pos;
    for (int i = 0; i < n; i++) {
        int j = std::lower_bound(pos.begin(), pos.end(), i, [&](int lhs, int rhs) {
            return compare(seq[lhs], seq[rhs]);
        }) - pos.begin();
        parent[i] = (j == 0 ? -1 : pos[j - 1]);
        if (j == std::ssize(pos)) {
            pos.push_back(i);
        } else if (seq[pos[j]] != seq[i]) {
            assert(compare(seq[i], seq[pos[j]]));
            pos[j] = i;
        }
    }
    std::vector<int> result;
    for (int i = pos.back(); i != -1;) {
        result.push_back(i);
        i = parent[i];
    }
    std::reverse(result.begin(), result.end());
    return result;
}

//  # longest_increasing_subseq(seq)
//  狭義単調増加な最長の部分列を1つ求め、その添字の列を返す
//  seqが空だと壊れる
//  O(n log n)
template <class T>
std::vector<int> longest_increasing_subseq(const std::vector<T>& seq) {
    return longest_increasing_subseq(seq, std::less<T>());
}

}  //  namespace nono
