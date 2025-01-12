#pragma once

#include <vector>

namespace nono {

///  brief : 二つのStackをくっつけてQueueにするやつ. 要素の総積を計算できる
template <class M>
class TwoStackQueue {
    using T = M::Value;

  public:
    TwoStackQueue(): prod_front_(1, M::e()), prod_back_(1, M::e()) {}

    ///  # front()
    ///  return data[0]
    ///  O(1)
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

    ///  # pop()
    ///  data <= data[1:]
    ///  O(1)
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

    ///  # push(elem)
    ///  data <= data + [elem]
    ///  O(1)
    void push(T elem) {
        back_.push_back(elem);
        prod_back_.push_back(M::op(prod_back_.back(), elem));
    }

    ///  # size()
    ///  |data|
    ///  O(1)
    int size() const {
        return front_.size() + back_.size();
    }

    ///  # prod()
    ///  op[for v in data](v)
    ///  O(1)
    T prod() const {
        return M::op(prod_front_.back(), prod_back_.back());
    }

    ///  # empty()
    ///  whether data is empty
    ///  O(1)
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
