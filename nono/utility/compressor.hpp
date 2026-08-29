#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

namespace nono {

///  # Compressor
template <class T>
class Compressor {
  public:
    Compressor() = default;

    Compressor(const std::vector<T>& data): data_(data) {
        std::sort(data_.begin(), data_.end());
        data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
    }
    Compressor(std::vector<T>&& data): data_(data) {
        std::sort(data_.begin(), data_.end());
        data_.erase(std::unique(data_.begin(), data_.end()), data_.end());
    }

    ///  # compress(value)
    ///  O(log n)
    int compress(const T& value) const {
        return std::distance(data_.begin(), std::lower_bound(data_.begin(), data_.end(), value));
    }

    ///  # compress(vec)
    ///  O(n log n)
    std::vector<int> compress(const std::vector<T>& vec) const {
        std::vector<int> result(vec.size());
        for (int i = 0; auto v: vec) {
            result[i] = compress(v);
            i++;
        }
        return result;
    }

    ///  # decompress(i)
    ///  O(1)
    T decompress(int i) const {
        assert(0 <= i && i < data_.size());
        return data_[i];
    }

    ///  # size()
    ///  |{input sequenve}|
    int size() const {
        return data_.size();
    }

    ///  # contains(value)
    ///  O(log n)
    bool contains(const T& value) const {
        auto it = std::ranges::lower_bound(data_, value);
        return it != data_.end() && *it == value;
    }

  private:
    std::vector<T> data_;
};

}  //  namespace nono
