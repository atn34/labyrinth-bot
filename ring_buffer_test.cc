#include <gtest/gtest.h>

#include "ring_buffer.h"

TEST(RingBuffer, Test) {
  RingBuffer<int, 5> buffer;
  EXPECT_EQ(0, buffer.size());
  buffer.add(0);
  EXPECT_EQ(0, buffer.get(0));
  EXPECT_EQ(1, buffer.size());

  for (const auto& i : {1, 2, 3, 4}) {
    buffer.add(i);
  }
  EXPECT_EQ(0, buffer.get(0));
  EXPECT_EQ(5, buffer.size());

  buffer.add(6);
  EXPECT_EQ(1, buffer.get(0));
  EXPECT_EQ(5, buffer.size());
}
