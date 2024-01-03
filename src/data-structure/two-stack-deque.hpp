#pragma once

#include <cassert>
#include <vector>

namespace nono {

template <class T, auto op, auto e>
class TwoStackDeque {
  public:
    TwoStackDeque(): prod_front_(1, e()), prod_back_(1, e()) {}

    T front() {
        if (front_.empty()) {
            int m = (back_.size() + 1) / 2;
            for (int i = m - 1; i >= 0; i--) {
                front_.push_back(back_[i]);
                prod_front_.push_back(op(prod_front_.back(), back_[i]));
            }
            back_.erase(back_.begin(), back_.begin() + m);
            prod_back_.resize(1, e());
            for (auto elem: back_) {
                prod_back_.push_back(op(elem, prod_back_.back()));
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
                prod_back_.push_back(op(front_[i], prod_back_.back()));
            }
            front_.erase(front_.begin(), front_.begin() + m);
            prod_front_.resize(1, e());
            for (auto elem: front_) {
                prod_front_.push_back(op(prod_front_.back(), elem));
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
                prod_front_.push_back(op(prod_front_.back(), back_[i]));
            }
            back_.erase(back_.begin(), back_.begin() + m);
            prod_back_.resize(1, e());
            for (auto elem: back_) {
                prod_back_.push_back(op(elem, prod_back_.back()));
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
                prod_back_.push_back(op(front_[i], prod_back_.back()));
            }
            front_.erase(front_.begin(), front_.begin() + m);
            prod_front_.resize(1, e());
            for (auto elem: front_) {
                prod_front_.push_back(op(prod_front_.back(), elem));
            }
        }
        assert(!back_.empty());
        back_.pop_back();
        prod_back_.pop_back();
    }

    void push_front(T elem) {
        front_.push_back(elem);
        prod_front_.push_back(op(prod_front_.back(), elem));
    }

    void push_back(T elem) {
        back_.push_back(elem);
        prod_back_.push_back(op(elem, prod_back_.back()));
    }

    int size() const {
        return front_.size() + back_.size();
    }

    T prod() const {
        return op(prod_back_.back(), prod_front_.back());
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
