#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "measure_ball_position.h"
#include "perspective_transform.h"
#include "ring_buffer.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, 60);
  if (!cap.isOpened())
    return -1;
  namedWindow("Ball position", CV_WINDOW_AUTOSIZE);

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
  setIdentity(kf.measurementNoiseCov, Scalar::all(1e1));
  kf.measurementNoiseCov.at<float>(0, 0) = 4;
  kf.measurementNoiseCov.at<float>(1, 1) = 4;
  setIdentity(kf.errorCovPost, Scalar::all(1e-1));

  kf.statePre.at<float>(0) = measurement(0);
  kf.statePre.at<float>(1) = measurement(1);
  kf.statePre.at<float>(2) = 0;
  kf.statePre.at<float>(3) = 0;
  kf.statePre.at<float>(4) = 0;
  kf.statePre.at<float>(5) = 0;

  constexpr int cam_latency_in_frames = 4;
  RingBuffer<Mat, cam_latency_in_frames + 1> estimate_buffer;

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      continue;
    }

    Point measured;
    if (!MeasureBallPosition(transformed, &measured)) {
      continue;
    }

    measurement(0) = measured.x;
    measurement(1) = measured.y;

    kf.predict();
    Mat estimated = kf.correct(measurement).clone();
    estimate_buffer.add(estimated);

    if (estimate_buffer.size() == cam_latency_in_frames + 1) {
      float t_delta = cam_latency_in_frames;
      const Mat& old_estimate = estimate_buffer.get(0);
      float x = old_estimate.at<float>(0);
      float y = old_estimate.at<float>(1);
      float vx = old_estimate.at<float>(2);
      float vy = old_estimate.at<float>(3);
      float ax = old_estimate.at<float>(4);
      float ay = old_estimate.at<float>(5);
      Point currentPointEstimate(x + t_delta * vx + 0.5 * t_delta * t_delta * ax,
                      y + t_delta * vy + 0.5 * t_delta * t_delta * ay);

      circle(transformed, currentPointEstimate, 10, Scalar(0, 0, 255));
    }

    imshow("Ball position", transformed);

    if (cvWaitKey(1) > 0) {
      break;
    }
  }
  return 0;
}
