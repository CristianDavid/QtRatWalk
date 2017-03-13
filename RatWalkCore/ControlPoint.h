#ifndef RATWALKCORE_CONTROLPOINT_H
#define RATWALKCORE_CONTROLPOINT_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace RatWalkCore {

/*!
 * \brief ControlPoint has the information asociated with a captured point.
 */
class ControlPoint {
public:
   int CoorX; //!< X coordinate
   int CoorY; //!< Y coordinate
   int CoorXCorrected; //!< Corrected X coordinate
   int CoorYCorrected; //!< Corrected Y coordinate
   double Theta; //!< Angle asociated with this point
   double ThetaCorrected; //!< Corrected angle
   cv::Vec3b MarkerColor; //!< \todo What is this member for?
};

} // namespace RatWalkCore

#endif // RATWALKCORE_CONTROLPOINT_H
