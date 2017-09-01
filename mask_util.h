#ifndef MASK_UTIL_H
#define MASK_UTIL_H

#include <memory>

#include "opencv2/opencv.hpp"

std::unique_ptr<cv::Mat> get_mask_from_file(const std::string &file_name);

#endif /* MASK_UTIL_H */
