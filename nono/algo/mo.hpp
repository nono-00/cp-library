#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

namespace nono {

/// brief : Mo's algorithm. もおぉぉ.

//  {|add,erase|}(pos, is_end): pos番目の要素を{|加える,削除する|}, is_end がtrueなら末尾要素の{|追加, 削除|}
//  solve(pos): pos番目のクエリに回答する
template <int BLOCK, class Add, class Erase, class Solve>
void mo(const std::vector<std::pair<int, int>>& querys, Add add, Erase erase, Solve solve) {
    std::vector<int> index(querys.size());
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [&](int lhs, int rhs) {
        return std::pair{querys[lhs].first / BLOCK, querys[lhs].second * ((querys[lhs].first / BLOCK) & 1 ? -1 : 1)}
               < std::pair{querys[rhs].first / BLOCK, querys[rhs].second * ((querys[rhs].first / BLOCK) & 1 ? -1 : 1)};
    });
    const bool begin = false;
    const bool end = true;
    int left = 0;
    int right = 0;
    for (auto i: index) {
        while (right < querys[i].second) add(right++, end);
        while (querys[i].first < left) add(--left, begin);
        while (querys[i].second < right) erase(--right, end);
        while (left < querys[i].first) erase(left++, begin);
        solve(i);
    }
}

template <int MAXN, int MAXQ, class Add, class Erase, class Solve>
void mo(std::vector<std::pair<int, int>> query, Add add, Erase erase, Solve solve) {
    mo<std::max(1, static_cast<int>(MAXN / std::sqrt(MAXQ)))>(query, add, erase, solve);
}

}  //  namespace nono
