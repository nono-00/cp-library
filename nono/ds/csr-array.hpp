#pragma once

#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

namespace nono {

//  # CSRArray
//  Compressed Sparse Row Array.
//  行ごとに長さの違う二次元配列を、一次元に潰して持つ
template <class T>
class CSRArray {
    using iterator = std::vector<T>::iterator;
    using const_iterator = std::vector<T>::const_iterator;
    using subrange = std::ranges::subrange<iterator, iterator>;
    using const_subrange = std::ranges::subrange<const_iterator, const_iterator>;

  public:
    CSRArray() = default;

    //  # CSRArray(row_size, data)
    //  dataは (行番号, 値) の列。同じ行の値は、dataに現れた順に並ぶ
    //  行番号が [0, row_size) でないと壊れる
    //  O(row_size + |data|)
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

    //  # CSRArray(row_size, row, data)
    //  row[i] が data[i] の行番号。同じ行の値は、dataに現れた順に並ぶ
    //  rowとdataの長さが違うと壊れる。行番号が [0, row_size) でないと壊れる
    //  O(row_size + |data|)
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

    //  # operator[](i)
    //  i行目の値の列を返す
    //  0 <= i < size() でないと壊れる
    subrange operator[](int i) {
        assert(0 <= i && i < row_size_);
        return std::ranges::subrange(data_.begin() + indptr_[i], data_.begin() + indptr_[i + 1]);
    }

    const_subrange operator[](int i) const {
        assert(0 <= i && i < row_size_);
        return std::ranges::subrange(data_.begin() + indptr_[i], data_.begin() + indptr_[i + 1]);
    }

    //  # all_size()
    //  全ての行の要素数の合計
    int all_size() const {
        return data_.size();
    }

    //  # size()
    //  行数
    int size() const {
        return row_size_;
    }

  private:
    int row_size_;
    std::vector<int> indptr_;
    std::vector<T> data_;
};

}  //  namespace nono
