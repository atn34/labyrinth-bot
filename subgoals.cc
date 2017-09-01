#include "subgoals.h"

#include <algorithm>
#include <unordered_map>

#include "bfs.h"
#include "mask_util.h"

using namespace cv;

namespace {

struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return p.x ^ (p.y << 1);
    }
};

const std::vector<Point> &subgoals() {
  static const std::vector<Point> &result = []() {
    const Point start(346, 29);
    const Point finish(620, 249);

    std::unordered_map<Point, Point, PointHasher> parents;

    auto img = get_mask_from_file("path.png");
    bitwise_not(*img, *img);

    Mat path = imread("path.png");

    DoBfs(img.get(), start, [&](Point p, Point parent) {
      parents[p] = parent;
      return p != finish;
    });

    auto *out = new std::vector<Point>;

    int iter = 0;
    for (auto p = parents.find(finish);
         p != parents.end() && p->second != start;
         p = parents.find(p->second)) {
      if (iter++ % 10 == 0) {
        out->push_back(p->second);
      }
    }
    out->push_back(start);

    std::reverse(out->begin(), out->end());
    return *out;
  }();
  return result;
}
}  // namespace

Point Subgoals::next_goal(Point ball_pos) {
  for (int i = subgoals().size() - 1; i > next_goal_index_; --i) {
    const auto &sub_goal = subgoals()[i];
    if ((ball_pos.x - sub_goal.x) * (ball_pos.x - sub_goal.x) +
            (ball_pos.y - sub_goal.y) * (ball_pos.y - sub_goal.y) <
        20 * 20) {
      next_goal_index_ = std::min(i + 1, static_cast<int>(subgoals().size() - 1));
      break;
    }
  }
  return subgoals()[next_goal_index_];
}
