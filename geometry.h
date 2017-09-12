#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <functional>
#include <iostream>
#include <math.h>

struct Vec2 {
  float x;
  float y;

  Vec2 MakeUnit();
  float theta() const;
  float MagnitudeSquared() const;
  float Magnitude() const;

  Vec2 &operator+=(const Vec2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  Vec2 &operator-=(const Vec2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  Vec2 &operator*=(float other) {
    x *= other;
    y *= other;
    return *this;
  }
};

inline float WrapAngle(float theta) { return atan2(sin(theta), cos(theta)); }

inline Vec2 UnitVec2FromTheta(float theta) {
  return Vec2{cosf(theta), sinf(theta)};
}

inline Vec2 Vec2FromInt(int x, int y) {
  return Vec2{static_cast<float>(x), static_cast<float>(y)};
}

inline std::ostream &operator<<(std::ostream &stream, const Vec2 &p) {
  stream << p.x << ", " << p.y;
  return stream;
}

struct Vec2Hasher {
  std::size_t operator()(const Vec2 &p) const {
    return std::hash<float>{}(p.x) ^ (std::hash<float>{}(p.y) << 1);
  }
};

inline Vec2 operator-(const Vec2 &p1, const Vec2 &p2) {
  return Vec2{p1.x - p2.x, p1.y - p2.y};
}

inline Vec2 operator+(const Vec2 &p1, const Vec2 &p2) {
  return Vec2{p1.x + p2.x, p1.y + p2.y};
}

inline Vec2 operator*(const Vec2 &p1, float scalar) {
  return Vec2{p1.x * scalar, p1.y * scalar};
}

inline bool operator==(const Vec2 &p1, const Vec2 &p2) {
  return p1.x == p2.x && p1.y == p2.y;
}

inline bool operator!=(const Vec2 &p1, const Vec2 &p2) { return !(p1 == p2); }

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
