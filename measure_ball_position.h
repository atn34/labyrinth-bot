#ifndef MEASURE_BALL_POSITION_H

#include "opencv2/opencv.hpp"

bool MeasureBallPosition(cv::Mat perspective_transformed,
                         cv::Point *measurement);

#define MEASURE_BALL_POSITION_H
#endif /* ifndef MEASURE_BALL_POSITION_H */
