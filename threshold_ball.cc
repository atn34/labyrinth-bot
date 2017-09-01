#include "threshold_ball.h"

#include "connected_components.h"
#include "mask_util.h"

using namespace cv;

namespace {

const Mat &get_ball_mask() {
  static const Mat &out = []() {
    return *get_mask_from_file("board.png").release();
  }();
  return out;
}

} // namespace

Mat ThresholdBall::Threshold(Mat transformed) const {
  std::vector<Mat> hsv_planes;
  split(transformed, hsv_planes);

  adaptiveThreshold(hsv_planes[0], hsv_planes[0], 255,
                    CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,
                    block_size_, ball_thresh_c_);

  Mat masked;
  hsv_planes[0].copyTo(masked, get_ball_mask());

  return masked;
}
