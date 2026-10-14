#pragma once

#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>
#include <vector>

namespace nono {

namespace manacher_internal {

//  # ManacherResult
//  manacherの結果
struct ManacherResult {
  public:
    ManacherResult(int n, std::vector<int> rad): n_(n), rad_(rad) {}

    //  # radius(i)
    //  s[i]を中心とする、奇数長の回文の最大半径(中心を含めて数える)
    //  回文の長さが欲しい場合は 2 * radius(i) - 1
    //  0 <= i < |s| でないと壊れる
    //  O(1)
    int radius(int i) {
        assert(0 <= i && i < n_);
        return rad_[2 * i + 1] / 2;
    }

    //  # radius(i, j)
    //  s[i]とs[j]の間を中心とする、偶数長の回文の最大半径。無ければ0
    //  回文の長さが欲しい場合は 2 * radius(i, j)
    //  j = i + 1 でないと壊れる
    //  O(1)
    int radius(int i, int j) {
        assert(0 <= i && i < n_);
        assert(0 <= j && j < n_);
        assert(i + 1 == j);
        return (rad_[2 * j] - 1) / 2;
    }

    //  # raw()
    //  各文字の間と両端に区切りを入れた、長さ 2 * |s| + 1 の列に対する半径の配列を返す
    //  添字 2 * i + 1 が s[i]、添字 2 * i が s[i - 1] と s[i] の間に対応する
    std::vector<int> raw() const {
        return rad_;
    }

  private:
    int n_;
    std::vector<int> rad_;
};

}  //  namespace manacher_internal

//  # manacher(seq)
//  各位置を中心とする最長の回文の半径を求める。奇数長・偶数長の両方に対応
//  O(|seq|)
template <std::ranges::random_access_range R>
manacher_internal::ManacherResult manacher(const R& seq) {
    std::vector<std::optional<typename R::value_type>> dummy;
    for (auto v: seq) {
        dummy.push_back(std::nullopt);
        dummy.push_back(v);
    }
    dummy.push_back(std::nullopt);

    int n = dummy.size();
    //  result[i]: s[i]を中心とした回文の最大半径
    std::vector<int> result(n);
    for (int i = 0, r = 0; i < n;) {
        while (0 <= 2 * i - r && r < n && dummy[r] == dummy[2 * i - r]) r++;
        result[i] = r - i;
        int j = i + 1;
        while (0 <= 2 * i - j && j < n && j + result[2 * i - j] < r) result[j] = result[2 * i - j], j++;
        i = j;
    }
    return {std::ssize(seq), result};
}

}  //  namespace nono
