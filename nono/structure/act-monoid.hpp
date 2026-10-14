#pragma once

//  「貼ったら使える」ための集約ヘッダ。個々のact-monoidはnono/structure/act-monoid/
//  以下に1ファイル1構造体で分割されている。テストや他ヘッダから特定のact-monoid
//  だけを使う場合は、verify時の依存範囲を絞るためにこちらではなく個別ファイルを
//  直接includeすることを推奨する。

#include "./act-monoid/act-monoid.hpp"
#include "./act-monoid/range-add-count-max.hpp"
#include "./act-monoid/range-add-count-min.hpp"
#include "./act-monoid/range-add-range-gcd.hpp"
#include "./act-monoid/range-affine-range-sum.hpp"
#include "./act-monoid/range-chmin-chmax-add-range-min-max.hpp"
#include "./act-monoid/range-update-max-sub-seq.hpp"
#include "./act-monoid/range-update-range-gcd.hpp"
#include "./act-monoid/rev.hpp"

namespace nono {

namespace act_monoid {

//  # ActMonoidTemplate
//  act-monoidが満たすべきインターフェース（説明用。実体としては使わない）
//  (Value, op, e) と (Act, composition, id) がそれぞれモノイドで、
//  mapping(act, op(a, b)) = op(mapping(act, a), mapping(act, b)) が成り立つこと
template <class T>
struct ActMonoidTemplate {
    struct Value {};
    struct Act {};
    static Value op(Value lhs, Value rhs);
    static Value e();
    static Value mapping(Act act, Value value);
    //  lhs(rhs(x))
    static Act composition(Act lhs, Act rhs);
    static Act id();
};

}  //  namespace act_monoid

}  //  namespace nono
