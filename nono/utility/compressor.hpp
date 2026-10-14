#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

namespace nono {

//  # Compressor
//  座標圧縮。渡した列をソートして重複を除き、値と順位(0始まり)を相互に変換する
template <class T>
class Compressor {
  public:
    Compressor() = default;

    //  # Compressor(data)
    //  O(n log n)
    Compressor(const std::vector<T>& data): data_(data) {
        std::sort(data_.begin(), data_.end());
        data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
    }
    Compressor(std::vector<T>&& data): data_(data) {
        std::sort(data_.begin(), data_.end());
        data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
    }

    //  # compress(value)
    //  value未満の値の種類数を返す。valueが元の列にあれば、その順位になる
    //  O(log n)
    int compress(const T& value) const {
        return std::distance(data_.begin(), std::lower_bound(data_.begin(), data_.end(), value));
    }

    //  # compress(vec)
    //  vecの各要素を compress(value) で変換した列を返す
    //  O(|vec| log n)
    std::vector<int> compress(const std::vector<T>& vec) const {
        std::vector<int> result(vec.size());
        for (int i = 0; auto v: vec) {
            result[i] = compress(v);
            i++;
        }
        return result;
    }

    //  # decompress(i)
    //  順位がiの値を返す
    //  0 <= i < size() でないと壊れる
    //  O(1)
    T decompress(int i) const {
        assert(0 <= i && i < data_.size());
        return data_[i];
    }

    //  # size()
    //  値の種類数
    int size() const {
        return data_.size();
    }

    //  # contains(value)
    //  valueが元の列に含まれるか
    //  O(log n)
    bool contains(const T& value) const {
        auto it = std::ranges::lower_bound(data_, value);
        return it != data_.end() && *it == value;
    }

  private:
    std::vector<T> data_;
};

}  //  namespace nono
