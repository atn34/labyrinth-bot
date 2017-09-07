#include <iostream>
#include <math.h>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "ball_state.h"
#include "measure_ball_position.h"
#include "motor_client.h"
#include "perspective_transform.h"
#include "subgoals.h"
#include "walls_and_holes.h"

using namespace cv;

int main(int, char **) {
  VideoCapture cap(1);
  cap.set(CV_CAP_PROP_FPS, 60);
  if (!cap.isOpened()) return -1;
  namedWindow("Controller", CV_WINDOW_AUTOSIZE);

  Mat src, transformed;

  BallState ball_state;

  Subgoals sub_goals;
  std::unique_ptr<MotorClient> motor_client;

  Vec2 goal = {};
  Vec2 accumulated_error = {};

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      std::cout << "perspective transform failed" << std::endl;
      break;
    }

    Vec2 measured;
    if (!MeasureBallPosition(transformed, &measured)) {
      std::cout << "measure ball position failed" << std::endl;
      break;
    }

    if (motor_client == nullptr) {
      motor_client.reset(new MotorClient);
    }

    ball_state.update(measured);

    int key = cvWaitKey(1);
    if (key == 'q') {
      break;
    }

    float x = ball_state.position().x;
    float y = ball_state.position().y;
    float vx = ball_state.velocity().x;
    float vy = ball_state.velocity().y;
    float ax = ball_state.acceleration().x;
    float ay = ball_state.acceleration().y;

    float px = 0;
    float py = 0;
    float dx = 0;
    float dy = 0;
    float ex = goal.x - x;
    float ey = goal.y - y;
    accumulated_error.x += ex;
    accumulated_error.y += ey;
    float ix = 0.02;
    float iy = 0.02;

    dx = 10;
    dy = 10;

    Vec2 next_goal = sub_goals.next_goal(Vec2{x, y});

    if (vx * vx + vy * vy >= 2) {
      accumulated_error.x = ex / ix;
      accumulated_error.y = ey / iy;
    }

    goal = next_goal;

    float frame_lookahead = 2;

    float px_term = ex * px;
    float py_term = ey * py;
    float ix_term = accumulated_error.x * ix;
    float iy_term = accumulated_error.y * iy;
    float dx_term = (vx + frame_lookahead * ax) * dx;
    float dy_term = (vy * frame_lookahead * ay) * dy;

    int motor_x = px_term + ix_term - dx_term;
    int motor_y = px_term + iy_term - dx_term;

    line(transformed, Point(x, y), Point(x, y) + Point(px_term, py_term),
         Scalar(0, 0, 255));
    line(transformed, Point(x, y), Point(x, y) - Point(dx_term, dy_term),
         Scalar(0, 255, 0));
    line(transformed, Point(x, y), Point(x, y) + Point(ix_term, iy_term),
         Scalar(255, 0, 0));

    float repel = 40000;
    for (const auto &hole : HoleCenters()) {
      float x_force = x - hole.x;
      float y_force = y - hole.y;

      float avoid_hole_x_adjustment =
          repel * x_force / (sqrt(x_force * x_force + y_force * y_force) *
                             (x_force * x_force + y_force * y_force));
      float avoid_hole_y_adjustment =
          repel * y_force / (sqrt(x_force * x_force + y_force * y_force) *
                             (x_force * x_force + y_force * y_force));

      line(transformed, Point(hole.x, hole.y),
           Point(hole.x, hole.y) +
               Point(avoid_hole_x_adjustment, avoid_hole_y_adjustment),
           Scalar(255, 255, 255));

      motor_x += avoid_hole_x_adjustment;
      motor_y += avoid_hole_y_adjustment;
    }

    motor_client->step_to(MotorClient::HORIZONTAL, motor_x);
    motor_client->step_to(MotorClient::VERTICAL, motor_y);

    circle(transformed, Point(goal.x, goal.y), 10, Scalar(0, 255, 0));

    imshow("Controller", transformed);
  }

  return 0;
}
