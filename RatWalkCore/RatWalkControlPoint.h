#ifndef RATWALKCONTROLPOINT_H
#define RATWALKCONTROLPOINT_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define NUMBEROFPOINTSTOTRACK 5

class RatWalkControlPoint {
public:
   int CoorX, CoorY;
   int CoorXCorrected, CoorYCorrected;
   double Theta;
   double ThetaCorrected;
   cv::Vec3b MarkerColor;
};

#endif // RATWALKCONTROLPOINT_H
