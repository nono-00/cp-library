#pragma once

/// TODO : split

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

using Real = long double;
constexpr Real EPS = 1e-12;
const Real PI = std::acos(-1);

int sign(Real x) {
    return x < -EPS ? -1 : x > EPS ? 1 : 0;
}

bool eq(Real lhs, Real rhs) {
    return sign(rhs - lhs) == 0;
}

bool lte(Real lhs, Real rhs) {
    return sign(rhs - lhs) >= 0;
}

bool lt(Real lhs, Real rhs) {
    return sign(rhs - lhs) > 0;
}

bool gte(Real lhs, Real rhs) {
    return lte(rhs, lhs);
}

bool gt(Real lhs, Real rhs) {
    return lt(rhs, lhs);
}

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
    friend std::ostream& operator<<(std::ostream& os, Point& p) {
        os << p.x << ' ' << p.y;
        return os;
    }
};

using Points = std::vector<Point>;
using Polygon = Points;
using Vector = Point;

Real norm(Vector p) {
    return p.x * p.x + p.y * p.y;
}

Real abs(Vector p) {
    return std::sqrt(norm(p));
}

Real dot(Vector lhs, Vector rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Real cross(Vector lhs, Vector rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

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

struct Circle {
    Real r;
    Point c;
    Circle() {}
    Circle(Real r, Point c): r(r), c(c) {}
};

struct Segment {
    Segment() {}
    Segment(Point p0, Point p1): p0(p0), p1(p1) {}
    Point p0, p1;
};

struct Line {
    Line() {}
    Line(Point p0, Point p1): p0(p0), p1(p1) {}
    explicit Line(Segment s): p0(s.p0), p1(s.p1) {}
    Point p0, p1;
};

Real distance(Point lhs, Point rhs) {
    return abs(rhs - lhs);
}

Real distance(Line l, Point p) {
    return std::abs(cross(l.p1 - l.p0, p - l.p0)) / abs(l.p1 - l.p0);
}

Real distance(Segment s, Point p) {
    if (lt(dot(s.p1 - s.p0, p - s.p0), 0)) {
        return distance(p, s.p0);
    }
    if (lt(dot(s.p0 - s.p1, p - s.p1), 0)) {
        return distance(p, s.p1);
    }
    return distance(Line(s), p);
}

bool intersect(Segment lhs, Segment rhs) {
    return ccw(lhs.p0, lhs.p1, rhs.p0) * ccw(lhs.p0, lhs.p1, rhs.p1) <= 0
           && ccw(rhs.p0, rhs.p1, lhs.p0) * ccw(rhs.p0, rhs.p1, lhs.p1) <= 0;
}

bool intersect(Segment s, Point p) {
    return ccw(s.p0, s.p1, p) == 0;
}

Real distance(Segment lhs, Segment rhs) {
    if (intersect(lhs, rhs)) {
        return Real(0);
    }
    return std::min({distance(lhs, rhs.p0), distance(lhs, rhs.p1), distance(rhs, lhs.p0), distance(rhs, lhs.p1)});
}

bool parallel(Vector lhs, Vector rhs) {
    return eq(cross(lhs, rhs), 0);
}

bool parallel(Segment lhs, Segment rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

bool parallel(Line lhs, Line rhs) {
    return parallel(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

Point crosspoint(Line lhs, Line rhs) {
    assert(!parallel(lhs, rhs));
    Real a = cross(lhs.p1 - lhs.p0, rhs.p1 - rhs.p0);
    Real b = cross(lhs.p1 - lhs.p0, lhs.p1 - rhs.p0);
    return rhs.p0 + (rhs.p1 - rhs.p0) * b / a;
}

bool intersect(Line l, Segment s) {
    return ccw(l.p0, l.p1, s.p0) * ccw(l.p0, l.p1, s.p1) <= 0;
}

bool intersect(Circle c, Line l) {
    return lte(distance(l, c.c), c.r);
}

bool contain(Circle c, Point p) {
    return lte(distance(c.c, p), c.r);
}

bool intersect(Circle c, Point p) {
    return eq(distance(c.c, p), c.r);
}

bool internal(Circle c, Point p) {
    return lt(distance(c.c, p), c.r);
}

//  円周、円内に交点が存在するかどうか
//  円内に線分を内包している場合もtrueと判定される
bool intersect(Circle c, Segment s) {
    return lte(distance(s, c.c), c.r);
}

//  4 -> 交差しない, 内包しない
//  3 -> 外接する
//  2 -> 交差する
//  1 -> 内接する
//  0 -> 内包する
int intersect(Circle lhs, Circle rhs) {
    if (gt(lhs.r, rhs.r)) std::swap(lhs, rhs);
    Real d = distance(lhs.c, rhs.c);
    if (lt(lhs.r + rhs.r, d)) return 4;
    if (eq(lhs.r + rhs.r, d)) return 3;
    if (gt(lhs.r + d, rhs.r)) return 2;
    if (eq(lhs.r + d, rhs.r)) return 1;
    return 0;
}

bool orthogonal(Vector lhs, Vector rhs) {
    return eq(dot(lhs, rhs), 0);
}

bool orthogonal(Segment lhs, Segment rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

bool orthogonal(Line lhs, Line rhs) {
    return orthogonal(lhs.p0 - lhs.p1, rhs.p0 - rhs.p1);
}

Point crosspoint(Segment lhs, Segment rhs) {
    assert(intersect(lhs, rhs));
    assert(!parallel(lhs, rhs));
    Real d0 = distance(Line(lhs.p0, lhs.p1), rhs.p0);
    Real d1 = distance(Line(lhs.p0, lhs.p1), rhs.p1);
    return rhs.p0 + (rhs.p1 - rhs.p0) * (d0 / (d0 + d1));
}

Real arg(Vector p) {
    return std::atan2(p.y, p.x);
}

Vector polar(Real a, Real r) {
    return Point(std::cos(r) * a, std::sin(r) * a);
}

//  ccw
Point rotate(Point p, Real t) {
    Point v = polar(1, t);
    return Point(p.x * v.x - p.y * v.y, p.x * v.y + p.y * v.x);
}

//  p0 - p1 - p2
//  小さいほう
Real angle(Point p0, Point p1, Point p2) {
    Real a = arg(p0 - p1);
    Real b = arg(p2 - p1);
    if (gt(a, b)) std::swap(a, b);
    return std::min(b - a, 2 * PI - (b - a));
}

Points crosspoint(Circle lhs, Circle rhs) {
    assert(intersect(lhs, rhs) != 0);
    assert(intersect(lhs, rhs) != 4);
    Real d = abs(lhs.c - rhs.c);
    if (eq(d, lhs.r + rhs.r)) return {lhs.c + lhs.r * (rhs.c - lhs.c) / d};
    Real a = std::acos((lhs.r * lhs.r + d * d - rhs.r * rhs.r) / (2 * lhs.r * d));
    Real t = arg(rhs.c - lhs.c);
    return {lhs.c + polar(lhs.r, t + a), lhs.c + polar(lhs.r, t - a)};
}

Point projection(Segment s, Point p) {
    Vector a = p - s.p0;
    Vector b = s.p1 - s.p0;
    Real t = dot(a, b) / norm(b);
    return s.p0 + t * b;
}

Point projection(Line l, Point p) {
    Vector a = p - l.p0;
    Vector b = l.p1 - l.p0;
    Real t = dot(a, b) / norm(b);
    return l.p0 + t * b;
}

Point reflection(Segment s, Point p) {
    return p + (projection(s, p) - p) * Real(2);
}

Point reflection(Line l, Point p) {
    return p + (projection(l, p) - p) * Real(2);
}

Points crosspoint(Circle c, Line l) {
    assert(intersect(c, l));
    Vector p = projection(l, c.c);
    Real t = std::sqrt(c.r * c.r - norm(c.c - p));
    if (eq(t, 0)) return {p};
    Vector e = (l.p1 - l.p0) / abs(l.p1 - l.p0);
    return {p + t * e, p + -t * e};
}

Points crosspoint(Circle c, Segment s) {
    assert(intersect(c, s));
    auto cps = crosspoint(c, Line(s));
    Points temp;
    for (auto cp: cps) {
        if (intersect(s, cp)) temp.push_back(cp);
    }
    return temp;
}

//  ccw -> +
//  cw -> -
Real area(Polygon poly) {
    int n = poly.size();
    Real result = 0;
    for (int i = 0; i < n; i++) {
        result += cross(poly[i], poly[(i + 1) % n]);
    }
    return result / 2;
}

Real perimeter(Polygon poly) {
    int n = poly.size();
    Real result = 0;
    for (int i = 0; i < n; i++) {
        result += abs(poly[i] - poly[(i + 1) % n]);
    }
    return result;
}

bool convex(Polygon poly) {
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        if (ccw(poly[i], poly[(i + 1) % n], poly[(i + 2) % n]) == -1) return false;
    }
    return true;
}

//  not require convex
//   0 -> not contain
//   1 -> on segment
//   2 -> internal
int contain(Polygon poly, Point p) {
    int n = poly.size();
    bool parity = false;
    for (int i = 0; i < n; i++) {
        Point a = poly[i] - p;
        Point b = poly[(i + 1) % n] - p;
        if (gt(a.y, b.y)) std::swap(a, b);
        if (lte(a.y, 0) && lt(0, b.y) && lt(cross(a, b), 0)) parity ^= true;
        if (eq(cross(a, b), 0) && lte(dot(a, b), 0)) return 1;
    }
    return (parity ? 2 : 0);
}

//  x座標最小, y座標最小の頂点から
//  半時計周りの凸包
Polygon convex_hull(Points points) {
    std::sort(points.begin(), points.end());
    if (points.size() < 3) return points;
    assert(points.size() >= 3);
    int n = points.size();
    Points lower, upper;
    lower.push_back(points[n - 1]);
    lower.push_back(points[n - 2]);
    upper.push_back(points[0]);
    upper.push_back(points[1]);
    for (int i = n - 3; i >= 0; i--) {
        for (int m = lower.size(); m >= 2 && ccw(lower[m - 2], lower[m - 1], points[i]) >= 0; m--) {
            lower.pop_back();
        }
        lower.push_back(points[i]);
    }
    for (int i = 2; i < n; i++) {
        for (int m = upper.size(); m >= 2 && ccw(upper[m - 2], upper[m - 1], points[i]) >= 0; m--) {
            upper.pop_back();
        }
        upper.push_back(points[i]);
    }
    std::reverse(lower.begin(), lower.end());
    assert(lower.front() == upper.front());
    assert(lower.back() == upper.back());
    std::copy(std::next(upper.rbegin()), std::prev(upper.rend()), std::back_inserter(lower));
    return lower;
}

//  凸多角形を線分で区切った左側
Polygon convex_cut(Polygon poly, Line l) {
    int n = poly.size();
    Polygon res;
    for (int i = 0; i < n; i++) {
        Point a = poly[i];
        Point b = poly[(i + 1) % n];
        if (ccw(l.p0, l.p1, a) != -1) res.push_back(a);
        if (ccw(l.p0, l.p1, a) * ccw(l.p0, l.p1, b) == -1) {
            res.push_back(crosspoint(l, Line(a, b)));
        }
    }
    return res;
}

//  二等分線
//  p0 - p1 - p2
//  小さいほう
Line bisector(Point p0, Point p1, Point p2) {
    return Line(p1, p1 + polar(1, angle(p0, p1, p2) / 2));
}

//  内接円
Circle incircle(Point p0, Point p1, Point p2) {
    assert(std::abs(ccw(p0, p1, p2)) == 1);
    Point c = crosspoint(bisector(p0, p1, p2), bisector(p1, p2, p0));
    Real r = std::abs(2 * area({p0, p1, p2}) / perimeter({p0, p1, p2}));
    return Circle(r, c);
}

//  点pを通る直線lの垂線
Line perpendicular(Line l, Point p) {
    Vector v = l.p1 - l.p0;
    return Line(p, p + Vector(-v.y, v.x));
}

//  垂直二等分線
Line perpendicular_bisector(Segment s) {
    return perpendicular(Line(s), (s.p0 + s.p1) / 2);
}

//  外接円
Circle circumscribed_circle(Point p0, Point p1, Point p2) {
    Point c = crosspoint(perpendicular_bisector(Segment(p0, p1)), perpendicular_bisector(Segment(p0, p2)));
    return Circle(distance(p0, c), c);
}

Real area(Circle c) {
    return c.r * c.r * PI;
}

//  点pを通る円cの接点
Points tangent(Circle c, Point p) {
    assert(lte(c.r, distance(c.c, p)));
    return crosspoint(c, Circle(std::sqrt(norm(c.c - p) - c.r * c.r), p));
}

//  多角形pが線分sを内包しているか
bool contain(Polygon ps, Segment s) {
    if (contain(ps, s.p0) == 0 || contain(ps, s.p1) == 0) return false;
    int n = ps.size();
    Points cps;
    cps.push_back(s.p0);
    cps.push_back(s.p1);
    for (int i = 0; i < n; i++) {
        Segment t(ps[i], ps[(i + 1) % n]);
        if (parallel(s, t)) continue;
        if (!intersect(s, t)) continue;
        Point cp = crosspoint(s, t);
        if (cp != s.p0 && cp != s.p1 && cp != t.p0 && cp != t.p1) return false;
        cps.push_back(cp);
    }
    std::sort(cps.begin(), cps.end());
    int m = cps.size();
    for (int i = 0; i + 1 < m; i++) {
        if (contain(ps, (cps[i] + cps[i + 1]) / 2) == 0) return false;
    }
    return true;
}

//  TODO
//   pq is convex hull and ccw
std::pair<Points, Points> lower_and_upper_convex_hull(Polygon ps) {
    auto it = std::max_element(ps.begin(), ps.end());
    Points lower(ps.begin(), it + 1);
    Points upper(it, ps.end());
    upper.push_back(ps.front());
    std::reverse(upper.begin(), upper.end());
    return {lower, upper};
}

Real y(Line l, Real x) {
    if (eq(l.p0.x, l.p1.x)) return std::min(l.p0.y, l.p1.y);
    return x * (l.p1.y - l.p0.y) / (l.p1.x - l.p0.x);
}
//  if (lhs.size() < 3 || rhs.size() < 3) return {};
//  auto [ll, lu] = lower_and_upper_convex_hull(lhs);
//  auto [rl, ru] = lower_and_upper_convex_hull(rhs);
//  constexpr Real INF = 1e18;
//  ll.push_back(Point(INF, INF));
//  lu.push_back(Point(INF, INF));
//  rl.push_back(Point(INF, INF));
//  ru.push_back(Point(INF, INF));

//  Points lower, upper;
//  for (int i = 0, j = 0, k = 0, l = 0; ;) {
//      if (ll[i] <= lu[j] && ll[i] <= rl[k] && ll[i] <= ru[l]) {
//          // ll[i]がrhsに含まれているか
//          if (0 < k && 0 < l) {
//              if (lte(y(Line(rl[k - 1], rl[k]), ll[i].x), ll[i].y) && lte(ll[i].y, y(Line(ru[l - 1], ru[l]),
//              ll[i].x))) {
//                  lower.push_back(ll[i]);
//              }
//          }
//          if (i + 2 < (int)ll.size()) {
//              Segment lls(ll[i], ll[i + 1]);
//              if (0 < k) {
//                  Segment rls(rl[k - 1], rl[k]);
//                  if (intersect(lls, rls) && !parallel(lls, rls)) {
//                      lower.push_back(crosspoint(lls, rls));
//                  }
//              }
//              if (0 < l) {
//                  Segment rus(ru[l - 1], ru[l]);
//                  if (intersect(lls, rus) && !parallel(lls, rus)) {
//                      lower.push_back(crosspoint(lls, rus));
//                      upper.push_back(crosspoint(lls, rus));
//                  }
//              }
//          }
//          i++;
//          if (i + 1 == (int)ll.size() && j + 1 == (int)lu.size()) break;
//      } else if (lu[j] <= ll[i] && lu[j] <= rl[k] && lu[j] <= ru[l]) {
//          // lu[j]がrhsに含まれているか
//          if (0 < k && 0 < l) {
//              if (lte(y(Line(rl[k - 1], rl[k]), lu[j].x), lu[j].y) && lte(lu[j].y, y(Line(ru[l - 1], ru[l]),
//              lu[j].x))) {
//                  upper.push_back(lu[j]);
//              }
//          }
//          if (j + 2 < (int)lu.size()) {
//              Segment lus(lu[j], lu[j + 1]);
//              if (0 < l) {
//                  Segment rus(ru[l - 1], ru[l]);
//                  if (intersect(lus, rus) && !parallel(lus, rus)) {
//                      upper.push_back(crosspoint(lus, rus));
//                  }
//              }
//              if (0 < k) {
//                  Segment rls(rl[k - 1], rl[k]);
//                  if (intersect(lus, rls) && !parallel(lus, rls)) {
//                      lower.push_back(crosspoint(lus, rls));
//                      upper.push_back(crosspoint(lus, rls));
//                  }
//              }
//          }
//          j++;
//          if (i + 1 == (int)ll.size() && j + 1 == (int)lu.size()) break;
//      } else if (rl[k] <= ll[i] && rl[k] <= lu[j] && rl[k] <= ru[l]) {
//          // rl[k]がlhsに含まれているか
//          if (0 < i && 0 < j) {
//              if (lte(y(Line(ll[i - 1], ll[i]), rl[k].x), rl[k].y) && lte(rl[k].y, y(Line(lu[j - 1], lu[j]),
//              rl[k].x))) {
//                  lower.push_back(rl[k]);
//              }
//          }
//          if (k + 2 < (int)rl.size()) {
//              Segment rls(rl[k], rl[k + 1]);
//              if (0 < i) {
//                  Segment lls(ll[i - 1], ll[i]);
//                  if (intersect(rls, lls) && !parallel(rls, lls)) {
//                      lower.push_back(crosspoint(rls, lls));
//                  }
//              }
//              if (0 < j) {
//                  Segment lus(lu[j - 1], lu[j]);
//                  if (intersect(rls, lus) && !parallel(rls, lus)) {
//                      lower.push_back(crosspoint(rls, lus));
//                      upper.push_back(crosspoint(rls, lus));
//                  }
//              }
//          }
//          k++;
//          if (k + 1 == (int)rl.size() && l + 1 == (int)ru.size()) break;
//      } else {
//          // ru[l]がlhsに含まれているか
//          if (0 < i && 0 < j) {
//              if (lte(y(Line(ll[i - 1], ll[i]), ru[l].x), ru[l].y) && lte(ru[l].y, y(Line(lu[j - 1], lu[j]),
//              ru[l].x))) {
//                  upper.push_back(ru[l]);
//              }
//          }
//          if (l + 2 < (int)ru.size()) {
//              Segment rus(ru[l], ru[l + 1]);
//              if (0 < j) {
//                  Segment lus(lu[j - 1], lu[j]);
//                  if (intersect(rus, lus) && !parallel(rus, lus)) {
//                      upper.push_back(crosspoint(rus, lus));
//                  }
//              }
//              if (0 < i) {
//                  Segment lls(ll[i - 1], ll[i]);
//                  if (intersect(rus, lls) && !parallel(rus, lls)) {
//                      lower.push_back(crosspoint(rus, lls));
//                      upper.push_back(crosspoint(rus, lls));
//                  }
//              }
//          }
//          l++;
//          if (k + 1 == (int)rl.size() && l + 1 == (int)ru.size()) break;
//      }
//  }
//  assert(lower.front() == upper.front());
//  assert(lower.back() == upper.back());
//  std::copy(std::next(upper.rbegin()), std::prev(upper.rend()), std::back_inserter(lower));
//  assert(convex(lower));
//  return lower;

//  require lhs and rhs convex and ccw
Polygon common_polygon(Polygon lhs, Polygon rhs) {
    if (lhs.size() < 3 || rhs.size() < 3) return {};
    auto [ll, lu] = lower_and_upper_convex_hull(lhs);
    auto [rl, ru] = lower_and_upper_convex_hull(rhs);
    constexpr Real INF = 1e18;
    ll.push_back(Point(INF, INF));
    lu.push_back(Point(INF, INF));
    rl.push_back(Point(INF, INF));
    ru.push_back(Point(INF, INF));

    Points lower, upper;
    for (int i = 0, j = 0, k = 0, l = 0;;) {
        if (ll[i] <= lu[j] && ll[i] <= rl[k] && ll[i] <= ru[l]) {
            //  ll[i]がrhsに含まれているか
            if (0 < k && 0 < l) {
                if (lte(y(Line(rl[k - 1], rl[k]), ll[i].x), ll[i].y)
                    && lte(ll[i].y, y(Line(ru[l - 1], ru[l]), ll[i].x))) {
                    lower.push_back(ll[i]);
                }
            }
            if (i + 2 < (int)ll.size()) {
                Segment lls(ll[i], ll[i + 1]);
                if (0 < k) {
                    Segment rls(rl[k - 1], rl[k]);
                    if (intersect(lls, rls) && !parallel(lls, rls)) {
                        lower.push_back(crosspoint(lls, rls));
                    }
                }
                if (0 < l) {
                    Segment rus(ru[l - 1], ru[l]);
                    if (intersect(lls, rus) && !parallel(lls, rus)) {
                        lower.push_back(crosspoint(lls, rus));
                        upper.push_back(crosspoint(lls, rus));
                    }
                }
            }
            i++;
            if (i + 1 == (int)ll.size() && j + 1 == (int)lu.size()) break;
        } else if (lu[j] <= ll[i] && lu[j] <= rl[k] && lu[j] <= ru[l]) {
            //  lu[j]がrhsに含まれているか
            if (0 < k && 0 < l) {
                if (lte(y(Line(rl[k - 1], rl[k]), lu[j].x), lu[j].y)
                    && lte(lu[j].y, y(Line(ru[l - 1], ru[l]), lu[j].x))) {
                    upper.push_back(lu[j]);
                }
            }
            if (j + 2 < (int)lu.size()) {
                Segment lus(lu[j], lu[j + 1]);
                if (0 < l) {
                    Segment rus(ru[l - 1], ru[l]);
                    if (intersect(lus, rus) && !parallel(lus, rus)) {
                        upper.push_back(crosspoint(lus, rus));
                    }
                }
                if (0 < k) {
                    Segment rls(rl[k - 1], rl[k]);
                    if (intersect(lus, rls) && !parallel(lus, rls)) {
                        lower.push_back(crosspoint(lus, rls));
                        upper.push_back(crosspoint(lus, rls));
                    }
                }
            }
            j++;
            if (i + 1 == (int)ll.size() && j + 1 == (int)lu.size()) break;
        } else if (rl[k] <= ll[i] && rl[k] <= lu[j] && rl[k] <= ru[l]) {
            //  rl[k]がlhsに含まれているか
            if (0 < i && 0 < j) {
                if (lte(y(Line(ll[i - 1], ll[i]), rl[k].x), rl[k].y)
                    && lte(rl[k].y, y(Line(lu[j - 1], lu[j]), rl[k].x))) {
                    lower.push_back(rl[k]);
                }
            }
            if (k + 2 < (int)rl.size()) {
                Segment rls(rl[k], rl[k + 1]);
                if (0 < i) {
                    Segment lls(ll[i - 1], ll[i]);
                    if (intersect(rls, lls) && !parallel(rls, lls)) {
                        lower.push_back(crosspoint(rls, lls));
                    }
                }
                if (0 < j) {
                    Segment lus(lu[j - 1], lu[j]);
                    if (intersect(rls, lus) && !parallel(rls, lus)) {
                        lower.push_back(crosspoint(rls, lus));
                        upper.push_back(crosspoint(rls, lus));
                    }
                }
            }
            k++;
            if (k + 1 == (int)rl.size() && l + 1 == (int)ru.size()) break;
        } else {
            //  ru[l]がlhsに含まれているか
            if (0 < i && 0 < j) {
                if (lte(y(Line(ll[i - 1], ll[i]), ru[l].x), ru[l].y)
                    && lte(ru[l].y, y(Line(lu[j - 1], lu[j]), ru[l].x))) {
                    upper.push_back(ru[l]);
                }
            }
            if (l + 2 < (int)ru.size()) {
                Segment rus(ru[l], ru[l + 1]);
                if (0 < j) {
                    Segment lus(lu[j - 1], lu[j]);
                    if (intersect(rus, lus) && !parallel(rus, lus)) {
                        upper.push_back(crosspoint(rus, lus));
                    }
                }
                if (0 < i) {
                    Segment lls(ll[i - 1], ll[i]);
                    if (intersect(rus, lls) && !parallel(rus, lls)) {
                        lower.push_back(crosspoint(rus, lls));
                        upper.push_back(crosspoint(rus, lls));
                    }
                }
            }
            l++;
            if (k + 1 == (int)rl.size() && l + 1 == (int)ru.size()) break;
        }
    }
    if (lower.empty() && upper.empty()) return {};
    std::cerr << lhs.size() << ' ' << rhs.size() << ' ' << lower.size() << ' ' << upper.size() << std::endl;
    //  assert(lower.front() == upper.front());
    //  assert(lower.back() == upper.back());
    std::copy(std::next(upper.rbegin()), std::prev(upper.rend()), std::back_inserter(lower));
    assert(convex(lower));
    return lower;
}

bool intersect(Polygon lhs, Polygon rhs) {
    return lt(0, area(common_polygon(lhs, rhs)));
}

std::vector<Line> tangent(Circle c1, Circle c2) {
    std::vector<Line> ret;
    if (c1.r < c2.r) std::swap(c1, c2);
    Real g = norm(c1.c - c2.c);
    if (eq(g, 0)) return ret;
    Point u = (c2.c - c1.c) / sqrt(g);
    Point v = rotate(u, PI * 0.5);
    for (int s: {-1, 1}) {
        Real h = (c1.r + s * c2.r) / sqrt(g);
        if (eq(1 - h * h, 0)) {
            ret.emplace_back(c1.c + u * c1.r, c1.c + (u + v) * c1.r);
        } else if (1 - h * h > 0) {
            Point uu = u * h, vv = v * sqrt(1 - h * h);
            ret.emplace_back(c1.c + (uu + vv) * c1.r, c2.c - (uu + vv) * c2.r * s);
            ret.emplace_back(c1.c + (uu - vv) * c1.r, c2.c - (uu - vv) * c2.r * s);
        }
    }
    return ret;
}
