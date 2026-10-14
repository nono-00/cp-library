#pragma once

namespace nono {

namespace monoid {

//  # Rev
//  モノイドMについて、順方向の積(ord)と逆方向の積(rev)を同時に持つラッパー
//  opが可換でないモノイドで、列を逆向きに並べた積も必要なときに使う
template <class M>
struct Rev {
    using T = M::Value;
    struct Value {
        Value(): ord(M::e()), rev(M::e()) {}
        Value(T v_): ord(v_), rev(v_) {}
        T ord, rev;
    };
    static Value op(Value lhs, Value rhs) {
        lhs.ord = M::op(lhs.ord, rhs.ord);
        lhs.rev = M::op(rhs.rev, lhs.rev);
        return lhs;
    }
    static Value e() {
        return Value{};
    }
};

}  //  namespace monoid

}  //  namespace nono
