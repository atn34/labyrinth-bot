#ifndef WALLS_AND_HOLES_H
#define WALLS_AND_HOLES_H

#include <vector>

#include "geometry.h"

const std::vector<std::vector<Vec2>> &WallPolygons();
const std::vector<Vec2> &HoleCenters();

#endif /* WALLS_AND_HOLES_H */
