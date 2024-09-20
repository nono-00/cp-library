#pragma once

#include <cassert>
#include <vector>

namespace nono {

///  brief : 二つのStackをくっつけてDequeにするやつ. 要素の総積を計算できる
template <class M>
class TwoStackDeque {
    using T = M::Value;

  public:
    TwoStackDeque(): prod_front_(1, M::e()), prod_back_(1, M::e()) {}

    T front() {
        if (front_.empty()) {
            int m = (back_.size() + 1) / 2;
            for (int i = m - 1; i >= 0; i--) {
                front_.push_back(back_[i]);
                prod_front_.push_back(M::op(back_[i], prod_front_.back()));
            }
            back_.erase(back_.begin(), back_.begin() + m);
            prod_back_.resize(1, M::e());
            for (auto elem: back_) {
                prod_back_.push_back(M::op(prod_back_.back(), elem));
            }
        }
        assert(!front_.empty());
        return front_.back();
    }

    T back() {
        if (back_.empty()) {
            int m = (front_.size() + 1) / 2;
            for (int i = m - 1; i >= 0; i--) {
                back_.push_back(front_[i]);
                prod_back_.push_back(M::op(prod_back_.back(), front_[i]));
            }
            front_.erase(front_.begin(), front_.begin() + m);
            prod_front_.resize(1, M::e());
            for (auto elem: front_) {
                prod_front_.push_back(op(elem, prod_front_.back()));
            }
        }
        assert(!back_.empty());
        back_.back();
    }

    void pop_front() {
        if (front_.empty()) {
            int m = (back_.size() + 1) / 2;
            for (int i = m - 1; i >= 0; i--) {
                front_.push_back(back_[i]);
                prod_front_.push_back(M::op(back_[i], prod_front_.back()));
            }
            back_.erase(back_.begin(), back_.begin() + m);
            prod_back_.resize(1, M::e());
            for (auto elem: back_) {
                prod_back_.push_back(M::op(prod_back_.back(), elem));
            }
        }
        assert(!front_.empty());
        front_.pop_back();
        prod_front_.pop_back();
    }

    void pop_back() {
        if (back_.empty()) {
            int m = (front_.size() + 1) / 2;
            for (int i = m - 1; i >= 0; i--) {
                back_.push_back(front_[i]);
                prod_back_.push_back(M::op(prod_back_.back(), front_[i]));
            }
            front_.erase(front_.begin(), front_.begin() + m);
            prod_front_.resize(1, M::e());
            for (auto elem: front_) {
                prod_front_.push_back(M::op(elem, prod_front_.back()));
            }
        }
        assert(!back_.empty());
        back_.pop_back();
        prod_back_.pop_back();
    }

    void push_front(T elem) {
        front_.push_back(elem);
        prod_front_.push_back(M::op(elem, prod_front_.back()));
    }

    void push_back(T elem) {
        back_.push_back(elem);
        prod_back_.push_back(M::op(prod_back_.back(), elem));
    }

    int size() const {
        return front_.size() + back_.size();
    }

    T prod() const {
        return M::op(prod_front_.back(), prod_back_.back());
    }

    bool empty() const {
        return front_.empty() && back_.empty();
    }

  private:
    std::vector<T> front_;
    std::vector<T> back_;
    std::vector<T> prod_front_;
    std::vector<T> prod_back_;
};

}  //  namespace nono
