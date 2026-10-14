#pragma once

namespace nono {

//  # test_value_associativity<S>(a, b, c)
//  opの結合律 op(a, op(b, c)) = op(op(a, b), c) が成り立つか
//  以下の関数はどれも、値や作用を == で比べられないと使えない
template <class S>
bool test_value_associativity(auto a, auto b, auto c) {
    return S::op(a, S::op(b, c)) == S::op(S::op(a, b), c);
}

//  # test_value_identity<S>(value)
//  e()がopの単位元か。op(e(), value) = op(value, e()) = value が成り立つか
template <class S>
bool test_value_identity(auto value) {
    return S::op(S::e(), value) == S::op(value, S::e()) && S::op(S::e(), value) == value;
}

//  # test_act_associativity<S>(a, b, c)
//  compositionの結合律が成り立つか
template <class S>
bool test_act_associativity(auto a, auto b, auto c) {
    return S::composition(a, S::composition(b, c)) == S::composition(S::composition(a, b), c);
}

//  # test_act_identity<S>(act)
//  id()がcompositionの単位元か
template <class S>
bool test_act_identity(auto act) {
    return S::composition(S::id(), act) == S::composition(act, S::id()) && S::composition(S::id(), act) == act;
}

//  # test_distributive<S>(act, lhs, rhs)
//  分配則 mapping(act, op(lhs, rhs)) = op(mapping(act, lhs), mapping(act, rhs)) が成り立つか
template <class S>
bool test_distributive(auto act, auto lhs, auto rhs) {
    return S::mapping(act, S::op(lhs, rhs)) == S::op(S::mapping(act, lhs), S::mapping(act, rhs));
}

//  # test_value_inverse<S>(value)
//  inv(value)がvalueの逆元か。op(value, inv(value)) = op(inv(value), value) = e() が成り立つか
template <class S>
bool test_value_inverse(auto value) {
    return S::op(value, S::inv(value)) == S::op(S::inv(value), value) && S::op(S::inv(value), value) == S::e();
}

}  //  namespace nono
