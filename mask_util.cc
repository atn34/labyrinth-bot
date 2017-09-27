#include "mask_util.h"

#include "camera_properties.h"

using namespace cv;

std::unique_ptr<Mat> get_mask_from_file(const std::string &file_name) {
  Mat mask_src = imread(file_name);
  auto mask =
      std::unique_ptr<Mat>(new Mat(mask_src.rows, mask_src.cols, CV_8U));
  for (int r = 0; r < mask_src.rows; ++r) {
    for (int c = 0; c < mask_src.cols; ++c) {
      const auto &point = mask_src.at<Vec3b>(r, c);
      if (point[0] == 220 && point[1] == 0 && point[2] == 255) {
        mask->at<uchar>(r, c) = 0;
      } else {
        mask->at<uchar>(r, c) = 255;
      }
    }
  }
  Size size(CAMERA_WIDTH, CAMERA_HEIGHT);
  resize(*mask, *mask, size);
  return mask;
}
