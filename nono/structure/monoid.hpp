#pragma once

//  「貼ったら使える」ための集約ヘッダ。個々のモノイドはnono/structure/monoid/以下に
//  1ファイル1構造体で分割されている。テストや他ヘッダから特定のモノイドだけを
//  使う場合は、verify時の依存範囲を絞るためにこちらではなく個別ファイルを
//  直接includeすることを推奨する。

#include "./monoid/add.hpp"
#include "./monoid/composite.hpp"
#include "./monoid/gcd.hpp"
#include "./monoid/max-index.hpp"
#include "./monoid/max-sub-seq.hpp"
#include "./monoid/max.hpp"
#include "./monoid/min-index.hpp"
#include "./monoid/min-max.hpp"
#include "./monoid/min.hpp"
#include "./monoid/mul.hpp"
#include "./monoid/rev.hpp"

namespace nono {

namespace monoid {

//  # MonoidTemplate
//  モノイドが満たすべきインターフェース（説明用。実体としては使わない）
//  opは結合的で、e()はその単位元であること
struct MonoidTemplate {
    struct Value {};
    static Value op(Value lhs, Value rhs);
    static Value e();
};

}  //  namespace monoid

}  //  namespace nono
