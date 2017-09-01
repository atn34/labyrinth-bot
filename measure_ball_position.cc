#include "measure_ball_position.h"

#include "connected_components.h"
#include "threshold_ball.h"

using namespace cv;

bool MeasureBallPosition(Mat perspective_transformed,
                         Point *measurement) {
  static const ThresholdBall& threshold_ball = *new ThresholdBall;
  Mat masked = threshold_ball.Threshold(perspective_transformed);

  ConnectedComponentsVisitor visitor(&masked);

  struct ComponentInfo {
    float xs = 0;
    float ys = 0;
    int size = 0;
  };
  std::vector<ComponentInfo> infos;
  int my_label = -1;

  visitor.Visit([&](Point p, int label) {
    if (my_label != label) {
      ++my_label;
      infos.push_back(ComponentInfo{});
    }
    auto &info = infos.back();
    info.ys += (static_cast<float>(p.y));
    info.xs += (static_cast<float>(p.x));
    ++info.size;
  });

  if (infos.size() == 0) {
    return false;
  }

  std::sort(infos.begin(), infos.end(),
            [](const ComponentInfo &x, const ComponentInfo &y) {
              return x.size > y.size;
            });

  const auto &ball_info = infos[0];

  measurement->x = ball_info.xs / ball_info.size;
  measurement->y = ball_info.ys / ball_info.size;
  return true;
}
