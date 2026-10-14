#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

namespace nono {

//  # mo<BLOCK>(querys, add, erase, solve)
//  Mo's algorithm。半開区間のクエリ [querys[i].first, querys[i].second) を並べ替え、
//  区間を1要素ずつ伸び縮みさせながら全てのクエリに答える
//  add(pos, is_end)   : pos番目の要素を区間に加える。is_endがtrueなら右端、falseなら左端に加わる
//  erase(pos, is_end) : pos番目の要素を区間から除く。is_endがtrueなら右端、falseなら左端から除かれる
//  solve(i)           : 区間が querys[i] に一致した状態で呼ばれる。i番目のクエリに答える
//  BLOCKはクエリの左端を分けるブロックの幅
//  add, eraseを合計 O(n^2 / BLOCK + q * BLOCK) 回呼ぶ (n: 列の長さ, q: クエリ数)
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

//  # mo<MAXN, MAXQ>(query, add, erase, solve)
//  BLOCK = max(1, MAXN / sqrt(MAXQ)) として mo<BLOCK> を呼ぶ
//  MAXNに列の長さの上限、MAXQにクエリ数の上限を渡すと、
//  add, eraseの呼び出しは合計 O(MAXN * sqrt(MAXQ) + MAXQ) 回になる
template <int MAXN, int MAXQ, class Add, class Erase, class Solve>
void mo(std::vector<std::pair<int, int>> query, Add add, Erase erase, Solve solve) {
    mo<std::max(1, static_cast<int>(MAXN / std::sqrt(MAXQ)))>(query, add, erase, solve);
}

}  //  namespace nono
