#pragma once

#include <cassert>
#include <iterator>
#include <ranges>
#include <vector>

namespace nono {

///  # Lyndon factorize
///  Lyndon分解. min rotationとかができたりする
///  O(|seq|)
/// 
///  文字列がLyndon <=> 任意のSuffixよりも辞書順が小さい
///                <=> 任意の循環文字列よりも辞書順が小さい
///
///  Lyndon Factorize
///  分解後の各要素はLyndon.
///  列は辞書順で非増加になっている
///  末尾の要素が最も小さい
///
///  [result[i], result[i + 1])がLyndonになっている
///
///  例: seq = babaabaab
///      result = [0, 1, 3, 6, 9]
///      [b] [ab] [aab] [aab]
template <std::ranges::random_access_range R>
std::vector<int> lyndon_factorize(const R& seq) {
    int n = std::ssize(seq);
    std::vector<int> result;
    result.push_back(0);
    for (int l = 0, r = 1; l < n;) {
        assert(l < r);
        //  s[l:r]はlyndon
        int len = r - l;
        while (r < n && seq[r] == seq[r - len]) r++;
        if (r == n || seq[r] < seq[r - len]) {
            while (l + len <= r) result.push_back(l += len);
            r = l + 1;
        } else {
            r++;
        }
    }
    return result;
}

}  //  namespace nono
