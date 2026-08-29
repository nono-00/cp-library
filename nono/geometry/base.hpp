#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace nono {

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
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        os << p.x << ' ' << p.y;
        return os;
    }
};

using Points = std::vector<Point>;
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

bool parallel(Vector lhs, Vector rhs) {
    return eq(cross(lhs, rhs), 0);
}

bool orthogonal(Vector lhs, Vector rhs) {
    return eq(dot(lhs, rhs), 0);
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

Real distance(Point lhs, Point rhs) {
    return abs(rhs - lhs);
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

}  //  namespace nono
