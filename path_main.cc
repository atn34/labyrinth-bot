#include <iostream>

#include <unordered_map>
#include <utility>

#include "opencv2/opencv.hpp"

#include "bfs.h"
#include "mask_util.h"

using namespace cv;

struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return p.x ^ (p.y << 1);
    }
};

int main(int argc, char *argv[]) {
  const Point start(346, 29);
  const Point finish(620, 249);

  std::unordered_map<Point, Point, PointHasher> parents;

  auto img = get_mask_from_file("path.png");
  bitwise_not(*img, *img);

  Mat path = imread("path.png");

  DoBfs(img.get(), start, 
        [&](Point p, Point parent) {
          parents[p] = parent;
          return p != finish;
        });

  int iter = 0;
  for (auto p = parents.find(finish); p != parents.end() && p->second != start;
       p = parents.find(p->second)) {
    if (iter++ % 25 == 0) {
      circle(path, p->second, 10, Scalar(0, 0, 255));
    }
  }
  circle(path, start, 10, Scalar(0, 0, 255));

  imshow("path", path);
  cvWaitKey(0);

  return 0;
}
