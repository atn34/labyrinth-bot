#ifndef WALLS_AND_HOLES_H
#define WALLS_AND_HOLES_H

#include <vector>

struct MyPoint {
  int x;
  int y;
};

const std::vector<std::vector<MyPoint>> &WallPolygons();
const std::vector<MyPoint> &HoleCenters();

#endif /* WALLS_AND_HOLES_H */
