#ifndef FIND_PINK_CORNERS_H
#define FIND_PINK_CORNERS_H

#include "opencv2/imgproc.hpp"

constexpr int kPinkHueLow = 0;
constexpr int kPinkHueHigh = 20;
constexpr int kPinkErosionSize = 2;

class HueThresholder {
 public:
  HueThresholder()
      : HueThresholder(kPinkHueLow, kPinkHueHigh, kPinkErosionSize) {}

  HueThresholder(int hue_low, int hue_high, int erosion_size)
      : hue_low_(hue_low),
        hue_high_(hue_high),
        erosion_element_(cv::getStructuringElement(
            cv::MORPH_RECT,
            cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
            cv::Point(erosion_size, erosion_size))) {}

  cv::Mat thresh(const cv::Mat &src);

 private:
  int hue_low_;
  int hue_high_;
  cv::Mat erosion_element_;
};

bool FindPinkCorners(const cv::Mat &src,
                     std::vector<cv::Point2f> *pink_corners);

#endif /* FIND_PINK_CORNERS_H */
