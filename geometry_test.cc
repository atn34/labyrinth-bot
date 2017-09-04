#include "gtest/gtest.h"

#include "geometry.h"

TEST(DistanceToImpact, CircleCircleX) {
    Circle start{{2, 0}, 2};
    Vec dir{1, 0};
    Circle target{{10, 0}, 1};
    EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

TEST(DistanceToImpact, CircleCircleY) {
    Circle start{{0, 2}, 2};
    Vec dir{0, 1};
    Circle target{{0, 10}, 1};
    EXPECT_EQ(5, DistanceToImpact(start, dir, target));
}

