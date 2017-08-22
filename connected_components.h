#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include <unordered_set>
#include <assert.h>

#include "opencv2/imgproc.hpp"

class ConnectedComponentsVisitor {
public:
  ConnectedComponentsVisitor(const cv::Mat *img)
      : img_(img), num_rows_(img->rows), num_cols_(img->cols) {
    assert(img_->depth() == CV_8U);
  }

  template <typename Lambda> void Visit(Lambda f) {
    marked_for_visiting_.clear();
    component_label_ = 0;
    for (int row = 0; row < num_rows_; ++row) {
      for (int col = 0; col < num_cols_; ++col) {
        if (img_->at<uchar>(row, col) == 0) {
          continue;
        }
        int node = row * num_cols_ + col;
        if (marked_for_visiting_.find(node) != marked_for_visiting_.end()) {
          continue;
        }
        DFS(node, f);
        ++component_label_;
      }
    }
  }

private:
  template <typename Lambda> void DFS(int node, Lambda f) {
    to_visit_.clear();
    to_visit_.push_back(node);
    marked_for_visiting_.insert(node);
    while (to_visit_.size() > 0) {
      int current = to_visit_.back();
      to_visit_.pop_back();
      int row = current / num_cols_;
      int col = current % num_cols_;
      f(row, col, component_label_);
      for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
          if (img_->at<uchar>(row, col) == 0) {
            continue;
          }
          if (!(c >= 0 && c < num_cols_ && r >= 0 && r < num_rows_)) {
            continue;
          }
          int neighbor = r * num_cols_ + c;
          if (marked_for_visiting_.find(neighbor) !=
              marked_for_visiting_.end()) {
            continue;
          }
          to_visit_.push_back(neighbor);
          marked_for_visiting_.insert(neighbor);
        }
      }
    }
  }

  const cv::Mat *img_;
  int num_rows_;
  int num_cols_;
  std::unordered_set<int> marked_for_visiting_;
  int component_label_ = 0;
  std::vector<int> to_visit_;
};

#endif  // CONNECTED_COMPONENTS_H