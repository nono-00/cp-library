#pragma once

#include <limits>
#include <type_traits>

#include <atcoder/lazysegtree>

namespace nono {

//  todo:
//  - 自分で書く
//  - range add range sum などを用意する

template <class T>
    requires std::is_arithmetic_v<T>
using RangeAddRangeMinSegmentTree =
    atcoder::lazy_segtree<T, [](const T& lhs, const T& rhs) -> T { return (lhs < rhs ? lhs : rhs); },
                          []() -> T { return std::numeric_limits<T>::max(); }, T,
                          [](const T& f, const T& x) -> T { return (x == std::numeric_limits<T>::max() ? x : f + x); },
                          [](const T& lhs, const T& rhs) -> T { return lhs + rhs; },
                          []() -> T {
                              return static_cast<T>(0);
                          }>;

template <class T>
    requires std::is_arithmetic_v<T>
using RangeAddRangeMaxSegmentTree =
    atcoder::lazy_segtree<T, [](const T& lhs, const T& rhs) -> T { return (lhs > rhs ? lhs : rhs); },
                          []() -> T { return std::numeric_limits<T>::min(); }, T,
                          [](const T& f, const T& x) -> T { return (x == std::numeric_limits<T>::min() ? x : f + x); },
                          [](const T& lhs, const T& rhs) -> T { return lhs + rhs; },
                          []() -> T {
                              return static_cast<T>(0);
                          }>;

template <class T>
    requires std::is_arithmetic_v<T>
using RangeSetRangeMinSegmentTree =
    atcoder::lazy_segtree<T, [](const T& lhs, const T& rhs) -> T { return (lhs < rhs ? lhs : rhs); },
                          []() -> T { return std::numeric_limits<T>::max(); }, T,
                          [](const T& f, const T& x) -> T { return (f == std::numeric_limits<T>::max() ? x : f); },
                          [](const T& lhs, const T& rhs) -> T {
                              return (lhs == std::numeric_limits<T>::max() ? rhs : lhs);
                          },
                          []() -> T {
                              return std::numeric_limits<T>::max();
                          }>;

template <class T>
    requires std::is_arithmetic_v<T>
using RangeSetRangeMaxSegmentTree =
    atcoder::lazy_segtree<T, [](const T& lhs, const T& rhs) -> T { return (lhs > rhs ? lhs : rhs); },
                          []() -> T { return std::numeric_limits<T>::min(); }, T,
                          [](const T& f, const T& x) -> T { return (f == std::numeric_limits<T>::min() ? x : f); },
                          [](const T& lhs, const T& rhs) -> T {
                              return (lhs == std::numeric_limits<T>::max() ? rhs : lhs);
                          },
                          []() -> T {
                              return std::numeric_limits<T>::min();
                          }>;

}  //  namespace nono
