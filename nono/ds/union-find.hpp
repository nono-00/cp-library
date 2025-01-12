#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

namespace nono {

///  # UnionFind
class UnionFind {
  public:
    UnionFind() = default;

    UnionFind(int size): size_(size), data_(size, -1) {}

    ///  # leader(x)
    ///  O(log N)
    int leader(int x) {
        assert(0 <= x && x < size_);
        if (data_[x] < 0) {
            return x;
        } else {
            return data_[x] = leader(data_[x]);
        }
    }

    ///  # merge(lhs, rhs)
    ///  edges <- edges + [(lhs, rhs)]
    ///  O(log n)
    bool merge(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        if (same(lhs, rhs)) {
            return false;
        }
        lhs = leader(lhs);
        rhs = leader(rhs);
        if (-data_[lhs] < -data_[rhs]) {
            std::swap(lhs, rhs);
        }
        data_[lhs] += data_[rhs];
        data_[rhs] = lhs;
        return true;
    }

    ///  # same(lhs, rhs)
    ///  whether lhs and rhs are in same group
    ///  O(log n)
    bool same(int lhs, int rhs) {
        assert(0 <= lhs && lhs < size_);
        assert(0 <= rhs && rhs < size_);
        return (leader(lhs) == leader(rhs));
    }

    ///  # size(x)
    ///  return |group x|
    ///  O(log n)
    int size(int x) {
        assert(0 <= x && x < size_);
        return -data_[leader(x)];
    }

    ///  # size()
    ///  num of vertex
    ///  O(1)
    int size() const {
        return size_;
    }

    ///  # groups()
    ///  https://atcoder.github.io/ac-library/document_ja/dsu.html
    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(size_), group_size(size_);
        for (int i = 0; i < size_; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(size_);
        for (int i = 0; i < size_; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < size_; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(std::remove_if(result.begin(), result.end(), [&](const std::vector<int>& v) { return v.empty(); }),
                     result.end());
        return result;
    }

  private:
    int size_;
    std::vector<int> data_;
};

}  //  namespace nono
