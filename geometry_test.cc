#include "gtest/gtest.h"

#include "geometry.h"

TEST(DistanceToImpact, CircleCircleX) {
  Circle start{{2, 0}, 2};
  Vec2 dir{1, 0};
  Circle target{{10, 0}, 1};
  EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleCircleY) {
  Circle start{{0, 2}, 2};
  Vec2 dir{0, 1};
  Circle target{{0, 10}, 1};
  EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleCircleNoImpact) {
  Circle start{{0, 2}, 2};
  Vec2 dir{1, 1};
  Circle target{{0, 10}, 1};
  EXPECT_LT(DistanceToImpact(start, dir, target), 0);
}

TEST(DistanceToImpact, CirclePerpendicularLineSegmentX) {
  Circle start{{2, 0}, 2};
  Vec2 dir{1, 0};
  LineSegment target{{10, -1}, {10, 1}};
  EXPECT_EQ(6, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CirclePerpendicularLineSegmentY) {
  Circle start{{0, 2}, 2};
  Vec2 dir{0, 1};
  LineSegment target{{-1, 10}, {1, 10}};
  EXPECT_EQ(6, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleParallelLineSegment) {
  Circle start{{2, 0}, 2};
  Vec2 dir{1, 0};
  LineSegment target{{9, 0}, {11, 0}};
  EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleParallelLineSegmentFlipped) {
  Circle start{{2, 0}, 2};
  Vec2 dir{1, 0};
  LineSegment target{{11, 0}, {9, 0}};
  EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleLineSegmentNoImpact) {
  Circle start{{2, 0}, 2};
  Vec2 dir{1, 1};
  LineSegment target{{10, -1}, {10, 1}};
  EXPECT_LT(DistanceToImpact(start, dir, target), 0);
}
