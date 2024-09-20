#pragma once

#include <vector>

namespace nono {

///  brief : 二つのStackをくっつけてQueueにするやつ. 要素の総積を計算できる
template <class M>
class TwoStackQueue {
    using T = M::Value;

  public:
    TwoStackQueue(): prod_front_(1, M::e()), prod_back_(1, M::e()) {}

    T front() {
        if (front_.empty()) {
            while (!back_.empty()) {
                auto elem = back_.back();
                back_.pop_back();
                prod_back_.pop_back();
                front_.push_back(elem);
                prod_front_.push_back(M::op(elem, prod_front_.back()));
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
                prod_front_.push_back(M::op(elem, prod_front_.back()));
            }
        }
        front_.pop_back();
        prod_front_.pop_back();
    }

    void push(T elem) {
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
