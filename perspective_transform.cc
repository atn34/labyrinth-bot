#include "perspective_transform.h"

#include "find_pink_corners.h"

#include "camera_properties.h"

using namespace cv;

namespace {

const std::vector<Point2f> &dst_coords() {
  static const std::vector<Point2f> *result = []() {
    auto *dst = new std::vector<Point2f>;
    dst->push_back(Point(10 * CAMERA_WIDTH / 640, 10 * CAMERA_HEIGHT / 480));
    dst->push_back(Point(CAMERA_WIDTH - (10 * CAMERA_HEIGHT / 480),
                         10 * CAMERA_WIDTH / 640));
    dst->push_back(Point(CAMERA_WIDTH - (10 * CAMERA_WIDTH / 640),
                         CAMERA_HEIGHT - (10 * CAMERA_HEIGHT / 480)));
    dst->push_back(Point(10 * CAMERA_WIDTH / 640,
                         CAMERA_HEIGHT - (10 * CAMERA_HEIGHT / 480)));
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
