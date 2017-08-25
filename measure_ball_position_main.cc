#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "connected_components.h"
#include "perspective_transform.h"
#include "threshold_ball.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  if (!cap.isOpened())
    return -1;
  namedWindow("Ball position", CV_WINDOW_AUTOSIZE);

  ThresholdBall threshold_ball;

  Mat src, transformed, masked, ball_coords;

  // Based on http://opencvexamples.blogspot.com/2014/01/kalman-filter-implementation-tracking.html
  KalmanFilter kf(/* dimension of state */ 6, /* dimension of measurement */ 2);
  /* clang-format off */
  kf.transitionMatrix = (Mat_<float>(6, 6)
   << 1,0,1,0,.5, 0,
      0,1,0,1, 0,.5,
      0,0,1,0, 1, 0,
      0,0,0,1, 0, 1,
      0,0,0,0, 1, 0,
      0,0,0,0, 0, 1);
  /* clang-format on */

  Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));

  setIdentity(kf.measurementMatrix);
  setIdentity(kf.processNoiseCov, Scalar::all(1e-4));
  setIdentity(kf.measurementNoiseCov, Scalar::all(1e-2));
  setIdentity(kf.errorCovPost, Scalar::all(.1));

  bool initialized_kf = false;
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
      float xs = 0;
      float ys = 0;
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
      info.ys += (static_cast<float>(row));
      info.xs += (static_cast<float>(col));
      ++info.size;
    });

    if (infos.size() == 0) {
      continue;
    }

    std::sort(infos.begin(), infos.end(),
              [](const ComponentInfo &x, const ComponentInfo &y) {
                return x.size > y.size;
              });

    const auto &ball_info = infos[0];
    measurement(0) = ball_info.xs / ball_info.size;
    measurement(1) = ball_info.ys / ball_info.size;

    if (!initialized_kf) {
      kf.statePre.at<float>(0) = measurement(0);
      kf.statePre.at<float>(1) = measurement(1);
      kf.statePre.at<float>(2) = 0;
      kf.statePre.at<float>(3) = 0;
      kf.statePre.at<float>(4) = 0;
      kf.statePre.at<float>(5) = 0;
      initialized_kf = true;
    } else {
      kf.predict();
      Mat estimated = kf.correct(measurement);
      Point statePt(estimated.at<float>(0), estimated.at<float>(1));

      circle(transformed, statePt, 10, Scalar(0, 0, 255));
    }

    imshow("Ball position", transformed);

    if (cvWaitKey(30) > 0) {
      break;
    }
  }
  return 0;
}
