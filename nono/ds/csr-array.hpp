#pragma once

#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

namespace nono {

///  # Compressed Sparse Row Array.
///  二次元配列を一次元に潰すやつ. 使ったことがない.
template <class T>
class CSRArray {
    using iterator = std::vector<T>::iterator;
    using const_iterator = std::vector<T>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    CSRArray() = default;

    CSRArray(int row_size, const std::vector<std::pair<int, T>>& data)
        : row_size_(row_size),
          indptr_(row_size_ + 1),
          data_(data.size()) {
        for (auto key: data | std::views::keys) {
            assert(0 <= key && key < row_size);
            indptr_[key + 1]++;
        }
        for (int i = 0; i < row_size_; i++) {
            indptr_[i + 1] += indptr_[i];
        }
        auto index = indptr_;
        for (const auto& [key, value]: data) {
            data_[index[key]++] = value;
        }
    }

    CSRArray(int row_size, const std::vector<int>& row, const std::vector<T>& data)
        : row_size_(row_size),
          indptr_(row_size_ + 1),
          data_(data.size()) {
        assert(row.size() == data.size());
        for (auto key: row) {
            assert(0 <= key && key < row_size);
            indptr_[key + 1]++;
        }
        for (int i = 0; i < row_size_; i++) {
            indptr_[i + 1] += indptr_[i];
        }
        auto index = indptr_;
        for (int i = 0; i < (int)data.size(); i++) {
            data_[index[row[i]]++] = data[i];
        }
    }

    subrange operator[](int i) {
        assert(0 <= i && i < row_size_);
        return std::ranges::subrange(data_.begin() + indptr_[i], data_.begin() + indptr_[i + 1]);
    }

    const_subrange operator[](int i) const {
        assert(0 <= i && i < row_size_);
        return std::ranges::subrange(data_.begin() + indptr_[i], data_.begin() + indptr_[i + 1]);
    }

    int all_size() const {
        return data_.size();
    }

    int size() const {
        return row_size_;
    }

  private:
    int row_size_;
    std::vector<int> indptr_;
    std::vector<T> data_;
};

}  //  namespace nono
