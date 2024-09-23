#pragma once

/// brief : 結合律などの確認
namespace nono {

// 結合律
template <class S>
bool test_associativity(auto a, auto b, auto c) {
    return S::op(a, S::op(b, c)) == S::op(S::op(a, b), c);
}

// 単位元
template <class S>
bool test_identity(auto value) {
    return S::op(S::e(), value) == S::op(value, S::e()) && S::op(S::e(), value) == value;
}

// 逆元
template <class S>
bool test_inverse(auto value) {
    return S::op(value, S::inv(value)) == S::op(S::inv(value), value) && S::op(S::inv(value), value) == S::e();
}

}  //  namespace nono
