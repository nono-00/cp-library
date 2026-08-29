#pragma once

#include <algorithm>
#include <cassert>
#include <set>

namespace nono {

///  # IntervalSet
///  集合を区間で管理するやつ
template <class Index = int>
class IntervalSet {
    struct Interval {
        Index left, right;
        friend bool operator<(Interval lhs, Interval rhs) {
            return lhs.left < rhs.left;
        }
        bool operator<(Index index) const {
            return left < index;
        }
        bool operator<=(Index index) const {
            return left <= index;
        }
        friend bool intersect(Interval lhs, Interval rhs) {
            return lhs.right > rhs.left && rhs.right > lhs.left;
        }
        bool contains(Index elem) const {
            return left <= elem && elem < right;
        }
    };

  public:
    using iterator = std::set<Interval>::iterator;
    using const_iterator = std::set<Interval>::const_iterator;

    ///  # IntervalSet()
    ///  空のinterval setを構築する
    ///  O(1)
    IntervalSet() {}

    ///  # add(left, right)
    ///  S <= S or [left, right)
    ///  ならし O(log n)
    void add(Index left, Index right) {
        assert(left < right);
        Interval interval{left, right};
        auto it = dat_.upper_bound(interval);
        if (it != dat_.begin()) {
            it--;
            if (left <= it->right) {
                interval.left = it->left;
                interval.right = std::max(interval.right, it->right);
                erase_interval(*it);
            }
        }
        it = dat_.lower_bound(interval);
        while (it != dat_.end() && it->left <= interval.right) {
            interval.right = std::max(interval.right, it->right);
            auto nit = std::next(it);
            erase_interval(*it);
            it = nit;
        }
        add_interval(interval);
    }

    ///  # erase(left, right)
    ///  S <= S - [left, right)
    ///  ならし O(log n)
    void erase(Index left, Index right) {
        assert(left < right);
        auto it = dat_.upper_bound({left, right});
        if (it != dat_.begin()) {
            it--;
            if (it->contains(right - 1)) {
                Interval lhs{it->left, left};
                Interval rhs{right, it->right};
                erase_interval(*it);
                if (lhs.left < lhs.right) add_interval(lhs);
                if (rhs.left < rhs.right) add_interval(rhs);
                return;
            } else if (it->contains(left)) {
                Interval interval{it->left, left};
                erase_interval(*it);
                if (interval.left < interval.right) add_interval(interval);
            }
        }
        it = dat_.lower_bound({left, right});
        while (it != dat_.end() && it->left < right) {
            Interval interval{right, it->right};
            auto nit = std::next(it);
            erase_interval(*it);
            it = nit;
            if (interval.left < interval.right) add_interval(interval);
        }
    }

    ///  # contains(index)
    ///  whether index is in S
    ///  O(log n)
    bool contains(Index index) {
        auto it = dat_.upper_bound({index, index});
        if (it == dat_.begin()) return false;
        it--;
        return it->contains(index);
    }

    /// # empty()
    /// whether S is emtpy
    bool empty() {
        return dat_.empty();
    }

    /// # size()
    /// |S|
    Index size() {
        return size_;
    }

    /// # clear()
    /// S <= {}
    void clear() {
        dat_.clear();
        size_ = 0;
    }

    iterator begin() noexcept {
        return dat_.begin();
    }

    const_iterator begin() const noexcept {
        return dat_.begin();
    }

    iterator end() noexcept {
        return dat_.end();
    }

    const_iterator end() const noexcept {
        return dat_.end();
    }

  private:
    void add_interval(Interval interval) {
        size_ += interval.right - interval.left;
        dat_.insert(interval);
    }

    void erase_interval(Interval interval) {
        size_ -= interval.right - interval.left;
        assert(dat_.contains(interval));
        dat_.erase(interval);
    }

    Index size_ = 0;
    std::set<Interval> dat_;
};

}  //  namespace nono
