#ifndef RATWALKCONTROLPOINT_H
#define RATWALKCONTROLPOINT_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define NUMBEROFPOINTSTOTRACK 5

namespace RatWalkCore {

/*!
 * \brief ControlPoint tiene la información asociada a un punto capturado.
 */
class ControlPoint {
public:
   int CoorX; //!< Coordenada del punto en X
   int CoorY; //!< Coordenada del punto en Y
   int CoorXCorrected; //!< Coordenada en X corregida
   int CoorYCorrected; //!< Coordenada en Y corregida
   double Theta; //!< Ángulo formado en tomando el punto como vértice
   double ThetaCorrected; //!< Ángulo corregido
   cv::Vec3b MarkerColor; //!< \todo preguntar qué hace este miembro y documentar
};

} // namespace RatWalkCore

#endif // RATWALKCONTROLPOINT_H
