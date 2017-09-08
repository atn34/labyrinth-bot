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

  Vec2 goal = {640 / 2, 30};
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

    goal = sub_goals.next_goal(ball_state.position());

    Vec2 error = (goal - ball_state.position()).MakeUnit() *
        std::min<float>((goal - ball_state.position()).Magnitude() / 5, 5);

    accumulated_error += error;
    if (ball_state.velocity().MagnitudeSquared() >= 2) {
      accumulated_error *= 0.95;
    }

    Vec2 p{5, 5};
    Vec2 i{0.1, 0.1};
    Vec2 d{5, 5};

    float lookahead = 4;
    
    float px_term = error.x * p.x;
    float py_term = error.y * p.y;
    float ix_term = accumulated_error.x * i.x;
    float iy_term = accumulated_error.y * i.y;
    float dx_term = (ball_state.velocity().x + lookahead *
            ball_state.acceleration().x) * d.x;
    float dy_term = (ball_state.velocity().y + lookahead *
            ball_state.acceleration().y) * d.y;

    float motor_x = px_term + ix_term - dx_term;
    float motor_y = py_term + iy_term - dy_term;

    float x = ball_state.position().x;
    float y = ball_state.position().y;

    line(transformed, Point(x, y), Point(x, y) + Point(px_term, py_term),
         Scalar(0, 0, 255));
    line(transformed, Point(x, y), Point(x, y) - Point(dx_term, dy_term),
         Scalar(0, 255, 0));
    line(transformed, Point(x, y), Point(x, y) + Point(ix_term, iy_term),
         Scalar(255, 0, 0));

    float repel = 20000;
    for (const auto &hole : HoleCenters()) {
      Vec2 force = ball_state.position() - hole;

      float avoid_hole_x_adjustment =
          repel * force.x / (sqrt(force.x * force.x + force.y * force.y) *
                             (force.x * force.x + force.y * force.y));
      float avoid_hole_y_adjustment =
          repel * force.y / (sqrt(force.x * force.x + force.y * force.y) *
                             (force.x * force.x + force.y * force.y));

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
