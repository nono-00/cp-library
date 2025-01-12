#pragma once

#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>
#include <vector>

namespace nono {

namespace internal {

///  # manacher result
struct ManacherResult {
  public:
    ManacherResult(int n, std::vector<int> rad): n_(n), rad_(rad) {}

    ///  # radius(i)
    ///  奇数長半径
    ///  s[i]をcenterとする回文半径
    ///  回文の長さが欲しい場合は2 * radius(i) - 1
    ///  O(1)
    int radius(int i) {
        assert(0 <= i && i < n_);
        return rad_[2 * i + 1] / 2;
    }

    ///  # radius(i, i + 1)
    ///  偶数長
    ///  s[i], s[i + 1]を中心とした回文半径
    ///  回文の長さが欲しい場合は2 * radius(i)
    ///  O(1)
    int radius(int i, int j) {
        assert(0 <= i && i < n_);
        assert(0 <= j && j < n_);
        assert(i + 1 == j);
        return (rad_[2 * j] - 1) / 2;
    }

    ///  # raw()
    std::vector<int> raw() const {
        return rad_;
    }

  private:
    int n_;
    std::vector<int> rad_;
};

}  //  namespace internal

///  # manacher
///  回文半径を求める. 偶奇両対応.
///  O(|seq|)
template <std::ranges::random_access_range R>
internal::ManacherResult manacher(const R& seq) {
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
