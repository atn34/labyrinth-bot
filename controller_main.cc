#include <iostream>
#include <math.h>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "ball_state.h"
#include "measure_ball_position.h"
#include "motor_client.h"
#include "perspective_transform.h"
#include "ring_buffer.h"
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
  Vec2 motor_zeros = {};
  RingBuffer<Vec2, 16> motor_command_history;

  for (;;) {
    cap >> src;
    if (!src.data) {
      return -1;
    }

    flip(src, src, -1);

    if (!PerspectiveTransform(src, &transformed)) {
      std::cerr << "perspective transform failed" << std::endl;
      break;
    }

    Vec2 measured;
    if (!MeasureBallPosition(transformed, &measured)) {
      std::cerr << "measure ball position failed" << std::endl;
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

    bool touching_obstacle = false;
    goal = sub_goals.next_goal(ball_state.position(), &touching_obstacle);

    Vec2 error =
        (goal - ball_state.position()).MakeUnit() *
        std::min<float>((goal - ball_state.position()).Magnitude() / 5, 5);

    accumulated_error += error;
    if (ball_state.velocity().MagnitudeSquared() >= 2) {
      accumulated_error *= 0.9;
    }

    if (std::abs(ball_state.velocity().x) * 10 > 10 &&
        std::abs(ball_state.acceleration().x) * 100 < 2 &&
        motor_command_history.full()) {
      motor_zeros.x = motor_command_history.get(10).x;
    }
    if (std::abs(ball_state.velocity().y) * 10 > 10 &&
        std::abs(ball_state.acceleration().y) * 100 < 2 &&
        motor_command_history.full()) {
      motor_zeros.y = motor_command_history.get(10).y;
    }

    Vec2 p{5, 5};
    Vec2 i{0.1, 0.1};
    Vec2 d{15, 15};

    float px_term = error.x * p.x;
    float py_term = error.y * p.y;
    float ix_term = accumulated_error.x * i.x;
    float iy_term = accumulated_error.y * i.y;
    float dx_term = ball_state.velocity().x * d.x;
    float dy_term = ball_state.velocity().y * d.y;

    Vec2 motor_command = {px_term + ix_term - dx_term,
                          py_term + iy_term - dy_term};

    float x = ball_state.position().x;
    float y = ball_state.position().y;

    line(transformed, Point(x, y), Point(x, y) + Point(px_term, py_term),
         Scalar(0, 0, 255));
    line(transformed, Point(x, y), Point(x, y) - Point(dx_term, dy_term),
         Scalar(0, 255, 0));
    line(transformed, Point(x, y), Point(x, y) + Point(ix_term, iy_term),
         Scalar(255, 0, 0));

    float repel = 10000;
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

      motor_command.x += avoid_hole_x_adjustment;
      motor_command.y += avoid_hole_y_adjustment;
    }

    std::cout << touching_obstacle << ", ";
    std::cout << motor_command << ", ";
    std::cout << motor_zeros << ", ";
    std::cout << ball_state.position() << ", ";
    std::cout << ball_state.velocity() << ", ";
    std::cout << ball_state.acceleration() << std::endl;

    motor_command += motor_zeros;
    motor_command_history.add(motor_command);
    motor_client->step_to(MotorClient::HORIZONTAL, motor_command.x);
    motor_client->step_to(MotorClient::VERTICAL, motor_command.y);

    circle(transformed, Point(goal.x, goal.y), kBallRadius, Scalar(0, 255, 0));
    circle(transformed, Point(x, y), kBallRadius,
           touching_obstacle ? Scalar(0, 0, 0) : Scalar(255, 255, 255));

    imshow("Controller", transformed);
  }

  return 0;
}
