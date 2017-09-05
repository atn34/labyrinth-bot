#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include <assert.h>
#include <deque>
#include <unordered_set>

#include "opencv2/imgproc.hpp"

#include "bfs.h"
#include "geometry.h"

class ConnectedComponentsVisitor {
 public:
  ConnectedComponentsVisitor(const cv::Mat *img)
      : img_(img), num_rows_(img->rows), num_cols_(img->cols) {
    assert(img_->depth() == CV_8U);
  }

  template <typename Lambda>
  void Visit(Lambda f) {
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
        Bfs bfs(img_, &marked_for_visiting_, &to_visit_);
        bfs.bfs(Vec2FromInt(col, row), [&](Vec2 p, Vec2 /* parent */) {
          f(p, component_label_);
          return true;
        });
        ++component_label_;
      }
    }
  }

 private:
  const cv::Mat *img_;
  int num_rows_;
  int num_cols_;
  int component_label_ = 0;
  std::unordered_set<int> marked_for_visiting_;
  std::deque<int> to_visit_;
};

#endif  // CONNECTED_COMPONENTS_H
