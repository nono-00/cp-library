#pragma once

#include <numeric>
#include <vector>

namespace nono {

template <class T, auto op, auto e>
class TwoStackQueue {
  public:
    TwoStackQueue(): prod_front_(1, e()), prod_back_(1, e()) {}

    T front() {
        if (front_.empty()) {
            while (!back_.empty()) {
                auto elem = back_.back();
                back_.pop_back();
                prod_back_.pop_back();
                front_.push_back(elem);
                prod_front_.push_back(op(prod_front_.back(), elem));
            }
        }
        return front_.back();
    }

    void pop() {
        if (front_.empty()) {
            while (!back_.empty()) {
                auto elem = back_.back();
                back_.pop_back();
                prod_back_.pop_back();
                front_.push_back(elem);
                prod_front_.push_back(op(prod_front_.back(), elem));
            }
        }
        front_.pop_back();
        prod_front_.pop_back();
    }

    void push(T elem) {
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
