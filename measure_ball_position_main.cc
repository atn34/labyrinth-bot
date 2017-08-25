#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "connected_components.h"
#include "perspective_transform.h"
#include "threshold_ball.h"

using namespace cv;

float avg(const std::vector<float> &v) {
  float result = 0;
  int size = v.size();
  for (const auto &x : v) {
    result += x / size;
  }
  return result;
}

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Ball position", CV_WINDOW_AUTOSIZE);

  ThresholdBall threshold_ball;

  Mat src, transformed, masked, ball_coords;
  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      continue;
    }

    masked = threshold_ball.Threshold(transformed);

    ConnectedComponentsVisitor visitor(&masked);

    struct ComponentInfo {
      std::vector<float> xs;
      std::vector<float> ys;
      int size = 0;
    };
    std::vector<ComponentInfo> infos;
    int my_label = -1;

    visitor.Visit([&](int row, int col, int label) {
      if (my_label != label) {
        ++my_label;
        infos.push_back(ComponentInfo{});
      }
      auto &info = infos.back();
      info.ys.push_back(static_cast<float>(row));
      info.xs.push_back(static_cast<float>(col));
      ++info.size;
    });

    std::sort(infos.begin(), infos.end(),
              [](const ComponentInfo &x, const ComponentInfo &y) {
                return x.size > y.size;
              });

    if (infos.size() > 0) {
      const auto &ball_info = infos[0];
      circle(transformed, Point(avg(ball_info.xs), avg(ball_info.ys)), 10,
             Scalar(0, 0, 255));
    }

    imshow("Ball position", transformed);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  return 0;
}
