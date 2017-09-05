#include <iostream>

#include <unordered_map>
#include <utility>

#include "opencv2/opencv.hpp"

#include "bfs.h"
#include "geometry.h"
#include "mask_util.h"

using namespace cv;

int main(int argc, char *argv[]) {
  const Vec2 start{346, 29};
  const Vec2 finish{620, 249};

  std::unordered_map<Vec2, Vec2, Vec2Hasher> parents;

  auto img = get_mask_from_file("path.png");
  bitwise_not(*img, *img);

  Mat path = imread("path.png");

  DoBfs(img.get(), start, [&](Vec2 p, Vec2 parent) {
    parents.insert({p, parent});
    return p != finish;
  });

  int iter = 0;
  for (auto p = parents.find(finish); p != parents.end() && p->second != start;
       p = parents.find(p->second)) {
    if (iter++ % 25 == 0) {
      circle(path, Point(p->second.x, p->second.y), 10, Scalar(0, 0, 255));
    }
  }
  circle(path, Point(start.x, start.y), 10, Scalar(0, 0, 255));

  imshow("path", path);
  cvWaitKey(0);

  return 0;
}
