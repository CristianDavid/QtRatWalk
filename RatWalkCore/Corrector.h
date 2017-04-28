#ifndef RATWALKCORE_CORRECTOR_H
#define RATWALKCORE_CORRECTOR_H

#include <string>
#include <opencv2/opencv.hpp>

#include "RatWalkCore/Video.h"

namespace RatWalkCore {

class Corrector {
public:
   Corrector(std::vector<Video> &VideoToAnalyze);
   cv::Mat performCorrection(std::string targetFilename, int MinHessian);
   bool loadCalibrationParametersFromFile(const char *filename);
   bool saveCalibrationParametersToFile(const char *filename);
   cv::Mat getHLeft();
   cv::Mat getHMiddle();
   cv::Mat getHRight();
private:
   std::vector<Video> &VideoToAnalyze;
   cv::Mat HLeft, HMiddle, HRight;
   cv::Mat TargetImage, TargetImageGray,
           Image1,     Image2,       Image3,
           ImageLeft,  ImageMiddle,  ImageRight,
           ImageLeftG, ImageMiddleG, ImageRightG, TargetImageGrayG;
};

} // namespace RatWalkCore

#endif // RATWALKCORE_CORRECTOR_H
