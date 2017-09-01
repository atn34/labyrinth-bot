#ifndef BFS_H
#define BFS_H

#include <deque>
#include <unordered_set>

#include "opencv2/imgproc.hpp"

class Bfs {
public:
  Bfs(const cv::Mat *img, std::unordered_set<int> *marked_for_visiting,
      std::deque<int> *to_visit)
      : img_(img), num_rows_(img->rows), num_cols_(img->cols),
        marked_for_visiting_(marked_for_visiting), to_visit_(to_visit) {}
  virtual ~Bfs() = default;

  template <typename Lambda> void bfs(int node, Lambda f) {
    to_visit_->clear();
    to_visit_->push_back(node);
    marked_for_visiting_->insert(node);
    while (to_visit_->size() > 0) {
      int current = to_visit_->front();
      to_visit_->pop_front();
      int row = current / num_cols_;
      int col = current % num_cols_;
      for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
          if (img_->at<uchar>(row, col) == 0) {
            continue;
          }
          if (!(c >= 0 && c < num_cols_ && r >= 0 && r < num_rows_)) {
            continue;
          }
          int neighbor = r * num_cols_ + c;
          if (marked_for_visiting_->find(neighbor) !=
              marked_for_visiting_->end()) {
            continue;
          }
          to_visit_->push_back(neighbor);
          marked_for_visiting_->insert(neighbor);
          f(r, c, /* parent */ current);
        }
      }
    }
  }

private:
  const cv::Mat *img_;
  int num_rows_;
  int num_cols_;
  std::unordered_set<int> *marked_for_visiting_;
  std::deque<int> *to_visit_;
};

#endif /* BFS_H */
