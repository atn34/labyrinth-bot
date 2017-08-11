#include "find_pink_corners.h"

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
