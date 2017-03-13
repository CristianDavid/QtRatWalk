#ifndef RATWALKCONSTANTES_H
#define RATWALKCONSTANTES_H

#include <opencv2/opencv.hpp>

namespace RatWalkCore {
   extern cv::Mat HLeft,HMiddle,HRight;
   constexpr int NUMBER_OF_POINTS_TO_TRACK   = 5;
   constexpr int NUMBER_OF_ANGLES_CALCULATED = 5;
}

#endif // RATWALKCONSTANTES_H
