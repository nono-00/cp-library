#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

namespace nono {

//  brief:
//  - 座標圧縮.
//
template <class T>
class Compressor {
  public:
    Compressor() = default;

    //  brief:
    //  - コンストラクタ
    Compressor(const std::vector<T>& data): data_(data) {
        std::sort(data_.begin(), data_.end());
        data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
    }

    //  brief:
    //  - `v` を座標圧縮した結果を取得する
    //
    //  details:
    //  - 構成時に渡した要素の中で `v` 未満の要素数を取得する
    int compress(const T& value) const {
        return std::distance(data_.begin(), std::lower_bound(data_.begin(), data_.end(), value));
    }

    std::vector<int> compress(const std::vector<T>& vec) const {
        std::vector<int> result(vec.size());
        for (int i = 0; auto v: vec) {
            result[i] = compress(v);
            i++;
        }
        return result;
    }

    //  brief:
    //  - `Compressor` に追加された要素の `i` 番目に大きい要素を取得する
    T decompress(int i) const {
        assert(0 <= i && i < data_.size());
        return data_[i];
    }

    //  brief:
    //  - 構築後の要素数を取得する
    int size() const {
        return data_.size();
    }

  private:
    std::vector<T> data_;
};

}  //  namespace nono
