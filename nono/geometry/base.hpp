#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace nono {

//  # Real
//  座標・長さ・角度に使う実数型
using Real = long double;
//  # EPS
//  誤差の許容幅。geometry/以下の比較と判定は、差がEPS以下の値を等しいものとして扱う
//  EPSは絶対誤差なので、座標の桁が大きいほど計算誤差がEPSを超えやすくなる
constexpr Real EPS = 1e-12;
const Real PI = std::acos(-1);

//  # sign(x)
//  x < -EPS なら -1、x > EPS なら 1、それ以外は 0
int sign(Real x) {
    return x < -EPS ? -1 : x > EPS ? 1 : 0;
}

//  # eq(lhs, rhs)
//  |lhs - rhs| <= EPS
bool eq(Real lhs, Real rhs) {
    return sign(rhs - lhs) == 0;
}

//  # lte(lhs, rhs)
//  lhs <= rhs + EPS
bool lte(Real lhs, Real rhs) {
    return sign(rhs - lhs) >= 0;
}

//  # lt(lhs, rhs)
//  lhs < rhs - EPS
bool lt(Real lhs, Real rhs) {
    return sign(rhs - lhs) > 0;
}

//  # gte(lhs, rhs)
//  lhs >= rhs - EPS
bool gte(Real lhs, Real rhs) {
    return lte(rhs, lhs);
}

//  # gt(lhs, rhs)
//  lhs > rhs + EPS
bool gt(Real lhs, Real rhs) {
    return lt(rhs, lhs);
}

//  # Point
//  2次元の点。ベクトルとしても使う(Vectorは別名)
//  加減算、実数との乗除算、比較、入出力ができる
//  ==, != は各座標をeqで比べる。<, <=, >, >= は (x, y) の辞書順で、同じくEPSの誤差を許容する
//  k / p は p / k と同じ値になる
//  0で割ると壊れる
struct Point {
    Real x;
    Real y;
    Point(Real x = 0, Real y = 0): x(x), y(y) {}
    Point& operator+=(Point rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }
    Point& operator-=(Point rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }
    Point& operator*=(Real k) {
        this->x *= k;
        this->y *= k;
        return *this;
    }
    Point& operator/=(Real k) {
        assert(k != 0);
        this->x /= k;
        this->y /= k;
        return *this;
    }
    friend Point operator+(Point lhs, Point rhs) {
        return lhs += rhs;
    }
    friend Point operator-(Point lhs, Point rhs) {
        return lhs -= rhs;
    }
    friend Point operator*(Point p, Real k) {
        return p *= k;
    }
    friend Point operator/(Point p, Real k) {
        return p /= k;
    }
    friend Point operator*(Real k, Point p) {
        return p * k;
    }
    friend Point operator/(Real k, Point p) {
        return p / k;
    }
    friend bool operator==(Point lhs, Point rhs) {
        return eq(lhs.x, rhs.x) && eq(lhs.y, rhs.y);
    }
    friend bool operator!=(Point lhs, Point rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(Point lhs, Point rhs) {
        if (eq(lhs.x, rhs.x)) return lt(lhs.y, rhs.y);
        return lt(lhs.x, rhs.x);
    }
    friend bool operator<=(Point lhs, Point rhs) {
        return lhs == rhs || lhs < rhs;
    }
    friend bool operator>(Point lhs, Point rhs) {
        return rhs < lhs;
    }
    friend bool operator>=(Point lhs, Point rhs) {
        return rhs <= lhs;
    }
    friend std::istream& operator>>(std::istream& is, Point& p) {
        is >> p.x >> p.y;
        return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << p.x << ' ' << p.y;
        return os;
    }
};

using Points = std::vector<Point>;
using Vector = Point;

//  # norm(p)
//  長さの2乗
Real norm(Vector p) {
    return p.x * p.x + p.y * p.y;
}

//  # abs(p)
//  長さ
Real abs(Vector p) {
    return std::sqrt(norm(p));
}

//  # dot(lhs, rhs)
//  内積
Real dot(Vector lhs, Vector rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

//  # cross(lhs, rhs)
//  外積 lhs.x * rhs.y - lhs.y * rhs.x
//  rhsがlhsから見て反時計回りの側にあれば正
Real cross(Vector lhs, Vector rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

//  # parallel(lhs, rhs)
//  平行か。逆向きも平行とみなす。零ベクトルはどのベクトルとも平行になる
bool parallel(Vector lhs, Vector rhs) {
    return eq(cross(lhs, rhs), 0);
}

//  # orthogonal(lhs, rhs)
//  直交するか。零ベクトルはどのベクトルとも直交する
bool orthogonal(Vector lhs, Vector rhs) {
    return eq(dot(lhs, rhs), 0);
}

//  # ccw(p0, p1, p2)
//  p0からp1へ向かう線分に対する、p2の位置を返す
//   1 : 反時計回りの側(左側)
//  -1 : 時計回りの側(右側)
//   2 : 同一直線上で、p0より手前 (p2, p0, p1 の順)
//  -2 : 同一直線上で、p1より先 (p0, p1, p2 の順)
//   0 : 線分p0p1上(端点を含む)
int ccw(Point p0, Point p1, Point p2) {
    Vector a = p1 - p0;
    Vector b = p2 - p0;
    //  ccw
    if (gt(cross(a, b), 0)) return 1;
    //  cw
    if (lt(cross(a, b), 0)) return -1;
    //  2 - 0 - 1
    if (lt(dot(a, b), 0)) return 2;
    //  0 - 1 - 2
    if (lt(norm(a), norm(b))) return -2;
    //  0 - 2 - 1
    return 0;
}

//  # distance(lhs, rhs)
//  2点間の距離
Real distance(Point lhs, Point rhs) {
    return abs(rhs - lhs);
}

//  # arg(p)
//  偏角。[-π, π]
Real arg(Vector p) {
    return std::atan2(p.y, p.x);
}

//  # polar(a, r)
//  長さがa、偏角がrのベクトル
Vector polar(Real a, Real r) {
    return Point(std::cos(r) * a, std::sin(r) * a);
}

//  # rotate(p, t)
//  pを原点中心に、反時計回りにtラジアン回した点
Point rotate(Point p, Real t) {
    Point v = polar(1, t);
    return Point(p.x * v.x - p.y * v.y, p.x * v.y + p.y * v.x);
}

//  # angle(p0, p1, p2)
//  p1を頂点とする角 p0 p1 p2 の大きさ。小さいほうを返すので [0, π]
Real angle(Point p0, Point p1, Point p2) {
    Real a = arg(p0 - p1);
    Real b = arg(p2 - p1);
    if (gt(a, b)) std::swap(a, b);
    return std::min(b - a, 2 * PI - (b - a));
}

}  //  namespace nono
