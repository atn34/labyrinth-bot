#include "perspective_transform.h"

#include "find_pink_corners.h"

using namespace cv;

namespace {

const std::vector<Point2f> &dst_coords() {
  static const std::vector<Point2f> *result = []() {
    auto *dst = new std::vector<Point2f>;
    dst->push_back(Point(10, 10));
    dst->push_back(Point(630, 10));
    dst->push_back(Point(630, 470));
    dst->push_back(Point(10, 470));
    return dst;
  }();
  return *result;
}

}  // namespace

bool PerspectiveTransform(cv::Mat src, cv::Mat *dst) {
  std::vector<Point2f> src_corners;
  if (!FindPinkCorners(src, &src_corners)) {
    return false;
  }

  Mat transform = getPerspectiveTransform(src_corners, dst_coords());
  warpPerspective(src, *dst, transform, src.size());
  return true;
}
