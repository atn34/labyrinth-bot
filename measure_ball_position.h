#ifndef MEASURE_BALL_POSITION_H

#include "opencv2/opencv.hpp"

#include "geometry.h"

bool MeasureBallPosition(cv::Mat perspective_transformed,
                         Vec2 *measurement);

#define MEASURE_BALL_POSITION_H
#endif /* ifndef MEASURE_BALL_POSITION_H */
