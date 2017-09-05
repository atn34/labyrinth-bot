#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <functional>

struct Vec2 {
    float x;
    float y;

    Vec2 MakeUnit();
    float theta() const;
    float MagnitudeSquared() const;
    float Magnitude() const;
};

inline Vec2 Vec2FromInt(int x, int y) {
    return Vec2{static_cast<float>(x), static_cast<float>(y)};
}

struct Vec2Hasher {
    std::size_t operator()(const Vec2& p) const {
        return std::hash<float>{}(p.x) ^ (std::hash<float>{}(p.y) << 1);
    }
};

inline Vec2 operator-(const Vec2& p1, const Vec2& p2) {
    return Vec2{p1.x - p2.x, p1.y - p2.y};
}

inline Vec2 operator+(const Vec2& p1, const Vec2& p2) {
    return Vec2{p1.x + p2.x, p1.y + p2.y};
}

inline bool operator==(const Vec2& p1, const Vec2& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

inline bool operator!=(const Vec2& p1, const Vec2& p2) {
    return !(p1 == p2);
}

struct LineSegment {
    Vec2 p1;
    Vec2 p2;
};

struct Circle {
    Vec2 p;
    float r;
};

/// Returns the minimum distance |start| would have to move along |direction| in
/// order to touch |target|, or a negative value if |start| will not touch
/// |target|.  Preconditions: |start| is not touching |target|, and |direction|
/// is unit.
float DistanceToImpact(Circle start, Vec2 direction, Circle target);
float DistanceToImpact(Circle start, Vec2 direction, LineSegment target);

#endif /* GEOMETRY_H */
