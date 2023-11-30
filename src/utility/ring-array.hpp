#pragma once

#include <cassert>
#include <vector>

namespace nono {

template <class T>
class RingArray {
    using container = RingArray<T>;

  public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using difference_type = typename std::vector<T>::difference_type;
    using size_type = typename std::vector<T>::size_type;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::reverse_iterator;

    RingArray() = default;
    explicit RingArray(size_type size): size_(size), data_(size) {}
    RingArray(size_type size, const T& value)
        : size_(size),
          data_(size_, value) {}

    iterator begin() noexcept {
        return data_.begin();
    }

    iterator end() noexcept {
        return data_.end();
    }

    const_iterator begin() const noexcept {
        return data_.begin();
    }

    const_iterator end() const noexcept {
        return data_.end();
    }

    const_iterator cbegin() const noexcept {
        return data_.cbegin();
    }

    const_iterator cend() const noexcept {
        return data_.cend();
    }

    reverse_iterator rbegin() noexcept {
        return data_.rbegin();
    }

    reverse_iterator rbegin() const noexcept {
        return data_.rbegin();
    }

    const_reverse_iterator crbegin() const noexcept {
        return data_.crbegin();
    }

    reverse_iterator rend() noexcept {
        return data_.rend();
    }

    reverse_iterator rend() const noexcept {
        return data_.rend();
    }

    const_reverse_iterator crend() const noexcept {
        return data_.crend();
    }

    friend bool operator==(const container& lhs, const container& rhs) {
        return lhs.data_ == rhs.data_;
    }

    friend bool operator!=(const container& lhs, const container& rhs) {
        return !(lhs == rhs);
    }

    void swap(container& other) {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    size_type size() const noexcept {
        return data_.size();
    }

    size_type max_size() const noexcept {
        return data_.max_size();
    }

    bool empty() const noexcept {
        return data_.empty();
    }

    reference operator[](int pos) {
        if (pos < 0) {
            pos += size_;
        } else if (pos >= (int)size_) {
            pos -= size_;
        }
        assert(0 <= pos && pos < (int)size_);
        return data_[pos];
    }

    const_reference operator[](int pos) const {
        if (pos < 0) {
            pos += size_;
        } else if (pos >= (int)size_) {
            pos -= size_;
        }
        assert(0 <= pos && pos < (int)size_);
        return data_[pos];
    }

    void fill(const T& value) {
        data_.assign(size_, value);
    }

  private:
    size_type size_;
    std::vector<T> data_;
};

}  //  namespace nono
