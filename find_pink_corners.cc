#include <algorithm>

#include "find_pink_corners.h"

#include "connected_components.h"

using namespace cv;

Mat HueThresholder::thresh(const Mat &src) {
  Mat hsv;
  cvtColor(src, hsv, CV_BGR2HSV);

  std::vector<Mat> hsv_planes;
  split(hsv, hsv_planes);

  inRange(hsv_planes[0], hue_low_, hue_high_, hsv_planes[0]);

  erode(hsv_planes[0], hsv_planes[0], erosion_element_);

  return hsv_planes[0];
}

bool FindPinkCorners(const Mat &src, std::vector<Point2f> *pink_corners) {
  HueThresholder thresholder;
  Mat threshed = thresholder.thresh(src);
  ConnectedComponentsVisitor visitor(&threshed);

  struct ComponentInfo {
    float xs = 0;
    float ys = 0;
    int size = 0;
  };
  std::vector<ComponentInfo> infos;
  int my_label = -1;

  visitor.Visit([&](Vec2 p, int label) {
    if (my_label != label) {
      ++my_label;
      infos.push_back(ComponentInfo{});
    }
    auto &info = infos.back();
    info.ys += p.y;
    info.xs += p.x;
    ++info.size;
  });

  if (infos.size() < 4) {
    return false;
  }

  std::sort(infos.begin(), infos.end(),
            [](const ComponentInfo &x, const ComponentInfo &y) {
              return x.size > y.size;
            });
  if (infos.size() > 4) {
    infos.erase(infos.begin() + 4, infos.end());
  }

  std::vector<Point2f> result;
  for (auto &info : infos) {
    float x = static_cast<float>(info.xs / info.size);
    float y = static_cast<float>(info.ys / info.size);
    result.push_back(Point2f{x, y});
  }

  auto top_left = *std::min_element(
      result.begin(), result.end(),
      [](const Point2f &x, const Point2f &y) { return x.x + x.y < y.x + y.y; });

  auto top_right = *std::min_element(
      result.begin(), result.end(), [&](const Point2f &x, const Point2f &y) {
        return (src.cols - x.x) + x.y < (src.cols - y.x) + y.y;
      });

  auto bottom_right = *std::min_element(
      result.begin(), result.end(), [&](const Point2f &x, const Point2f &y) {
        return (src.cols - x.x) + (src.rows - x.y) <
               (src.cols - y.x) + (src.rows - y.y);
      });

  auto bottom_left = *std::min_element(
      result.begin(), result.end(), [&](const Point2f &x, const Point2f &y) {
        return x.x + (src.rows - x.y) < y.x + (src.rows - y.y);
      });

  *pink_corners = {top_left, top_right, bottom_right, bottom_left};
  return true;
}
