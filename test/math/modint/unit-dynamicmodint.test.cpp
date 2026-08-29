// competitive-verifier: STANDALONE
#include <cassert>
#include <sstream>

#include "nono/math/modint.hpp"

using Mint = nono::DynamicModint<0>;

namespace nono {

void solve() {
    Mint::set_mod(998244353);

    //  operator>>: 文字列を読み込んで剰余がとられた値になること
    {
        std::stringstream ss("0");
        Mint v;
        ss >> v;
        assert(v.val() == 0);
    }
    {
        std::stringstream ss("123");
        Mint v;
        ss >> v;
        assert(v.val() == 123);
    }
    {
        //  法以上の値は剰余がとられる
        std::stringstream ss("998244353");
        Mint v;
        ss >> v;
        assert(v.val() == 0);
    }
    {
        std::stringstream ss("1000000009");
        Mint v;
        ss >> v;
        assert(v.val() == 1000000009 % 998244353);
    }
    {
        //  負の値も正しく正規化される
        std::stringstream ss("-1");
        Mint v;
        ss >> v;
        assert(v.val() == 998244353 - 1);
    }

    //  operator<<: val()と同じ文字列が出力されること
    {
        std::stringstream ss;
        ss << Mint(123);
        assert(ss.str() == "123");
    }
    {
        std::stringstream ss;
        ss << Mint(0);
        assert(ss.str() == "0");
    }

    //  ラウンドトリップ: operator>>で読んだ値をoperator<<で出力すると一致する
    for (int x : {0, 1, 123456, 998244352}) {
        std::stringstream in;
        in << x;
        Mint v;
        in >> v;
        std::stringstream out;
        out << v;
        assert(out.str() == std::to_string(x));
    }
}

}  //  namespace nono

int main() {
    nono::solve();
}
