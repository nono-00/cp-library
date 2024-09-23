#pragma once

///  brief : 結合律などの確認
namespace nono {

//  結合律
template <class S>
bool test_value_associativity(auto a, auto b, auto c) {
    return S::op(a, S::op(b, c)) == S::op(S::op(a, b), c);
}

//  単位元
template <class S>
bool test_value_identity(auto value) {
    return S::op(S::e(), value) == S::op(value, S::e()) && S::op(S::e(), value) == value;
}

//  結合律
template <class S>
bool test_act_associativity(auto a, auto b, auto c) {
    return S::composition(a, S::composition(b, c)) == S::composition(S::composition(a, b), c);
}

//  単位元
template <class S>
bool test_act_identity(auto act) {
    return S::composition(S::id(), act) == S::composition(act, S::id()) && S::composition(S::id(), act) == act;
}

//  分配則
template <class S>
bool test_distributive(auto act, auto lhs, auto rhs) {
    return S::mapping(act, S::op(lhs, rhs)) == S::op(S::mapping(act, lhs), S::mapping(act, rhs));
}

//  逆元
template <class S>
bool test_value_inverse(auto value) {
    return S::op(value, S::inv(value)) == S::op(S::inv(value), value) && S::op(S::inv(value), value) == S::e();
}

}  //  namespace nono
