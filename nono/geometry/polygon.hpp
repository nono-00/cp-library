#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

#include "../utility/is-debug.hpp"
#include "./base.hpp"
#include "./segment.hpp"

namespace nono {

//  # Polygon
//  多角形。頂点を周に沿った順に並べた点列
using Polygon = Points;

//  # area(poly)
//  符号付き面積。頂点が反時計回りなら正、時計回りなら負
Real area(Polygon poly) {
    int n = poly.size();
    Real result = 0;
    for (int i = 0; i < n; i++) {
        result += cross(poly[i], poly[(i + 1) % n]);
    }
    return result / 2;
}

//  # perimeter(poly)
//  周長
Real perimeter(Polygon poly) {
    int n = poly.size();
    Real result = 0;
    for (int i = 0; i < n; i++) {
        result += abs(poly[i] - poly[(i + 1) % n]);
    }
    return result;
}

//  # convex(poly)
//  頂点が反時計回りに並んだ凸多角形か。連続する3頂点が同一直線上にあってもよい
//  時計回りに並んだ凸多角形はfalseになる
bool convex(Polygon poly) {
    int n = poly.size();
    for (int i = 0; i < n; i++) {
        if (ccw(poly[i], poly[(i + 1) % n], poly[(i + 2) % n]) == -1) return false;
    }
    return true;
}

//  # contain(poly, p)
//  多角形polyに対する点pの位置を返す。polyは凸でなくてよい
//  0 : 外部
//  1 : 辺上(頂点を含む)
//  2 : 内部
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

//  # convex_hull(points)
//  凸包。最も左の点(xが同じならyが小さい点)から反時計回りに並べて返す
//  辺上にある点も結果に含める。pointsが2点以下なら、ソートしただけの列を返す
//  pointsに同じ点が複数あると壊れる(結果に同じ点が重複して入る)
//  O(n log n)
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

//  # convex_cut(poly, l)
//  凸多角形polyを直線lで切り、l.p0からl.p1へ向かって左側の部分を返す。l上の点は左側に含める
//  polyが反時計回りの凸多角形でないと壊れる
Polygon convex_cut(Polygon poly, Line l) {
    DEBUG_ASSERT(convex(poly));
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

//  # contain(ps, s)
//  線分sが多角形psの内部と辺上に収まっているか。psは凸でなくてよい
//  O(n^2)
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

//  # lower_and_upper_convex_hull(ps)
//  凸多角形psを、先頭の点と最も右の点(xが同じならyが大きい点)で2本の折れ線に分ける
//  first  : 下側の折れ線。先頭の点から最も右の点まで
//  second : 上側の折れ線。先頭の点から最も右の点まで
//  psが convex_hull の返す形(最も左の点から始まる反時計回りの凸多角形)でないと壊れる
std::pair<Points, Points> lower_and_upper_convex_hull(Polygon ps) {
    DEBUG_ASSERT(convex(ps));
    auto it = std::max_element(ps.begin(), ps.end());
    Points lower(ps.begin(), it + 1);
    Points upper(it, ps.end());
    upper.push_back(ps.front());
    std::reverse(upper.begin(), upper.end());
    return {lower, upper};
}

//  # common_polygon(lhs, rhs)
//  凸多角形どうしの共通部分(凸多角形)
//  rhsの各辺でlhsを順にconvex_cutしていく(Sutherland-Hodgman法)
//  どちらかが2点以下なら空を返す
//  lhs, rhsが反時計回りの凸多角形でないと壊れる
//  O(m(n + m))  (n = |lhs|, m = |rhs|)
Polygon common_polygon(Polygon lhs, Polygon rhs) {
    DEBUG_ASSERT(convex(lhs) && convex(rhs));
    if (lhs.size() < 3 || rhs.size() < 3) return {};
    Polygon result = lhs;
    int m = rhs.size();
    for (int i = 0; i < m; i++) {
        result = convex_cut(result, Line(rhs[i], rhs[(i + 1) % m]));
    }
    return result;
}

//  # intersect(lhs, rhs)
//  凸多角形どうしの共通部分が正の面積を持つか。辺や頂点で接するだけならfalse
//  lhs, rhsが反時計回りの凸多角形でないと壊れる
bool intersect(Polygon lhs, Polygon rhs) {
    return lt(0, area(common_polygon(lhs, rhs)));
}

}  //  namespace nono
