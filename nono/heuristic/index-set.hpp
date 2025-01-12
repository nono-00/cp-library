#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  DEPRECETED
///  TODO : implementation
template <class T>
class IndexSet {
  public:
    using iterator = std::vector<T>::iterator;
    using const_iterator = std::vector<T>::const_iterator;

    IndexSet() {}
    IndexSet(const std::vector<T>& data): data_(data) {}

    void insert(T key) {
        data_.push_back(key);
    }

    bool empty() {
        return data_.empty();
    }

    int size() {
        return data_.size();
    }

    void erase(int pos) {
        assert(0 <= pos && pos < (int)data_.size());
        if (pos != (int)data_.size() - 1) {
            std::swap(data_.back(), data_[pos]);
        }
        data_.pop_back();
    }

    T& operator[](int pos) {
        assert(0 <= pos && pos < (int)data_.size());
        return data_[pos];
    }

    const T& operator[](int pos) const {
        assert(0 <= pos && pos < (int)data_.size());
        return data_[pos];
    }

    iterator begin() {
        return data_.begin();
    }

    const_iterator begin() const {
        return data_.begin();
    }

    iterator end() {
        return data_.end();
    }

    const_iterator end() const {
        return data_.end();
    }

  private:
    std::vector<T> data_;
};

}  //  namespace nono
