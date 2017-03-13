#ifndef RATWALKFRAMEOBJECT_H
#define RATWALKFRAMEOBJECT_H

#include <vector>
#include "RatWalkCore/ControlPoint.h"

namespace RatWalkCore {

/*!
 * \brief Esta clase representa un frame de un video con sus correspondientes
 * puntos de control
 *
 * \see RatWalkCore::ControlPoint
 *
 * \todo Mejorar encapsulamiento de la clase, diseñar la interfaz de tal modo
 *  que sus miembros siempre tenga congruencia
 */
class Frame {
public:
   bool IsSet=0;
   int FrameNumber;
   std::vector<ControlPoint> TrackedPointsInFrame;
   int NumberOfTRegisteredPoints=0;
   int NumberOfPointsToTrack;

   /*!
    * \brief SetNumberOfNumberOfPointsToTrack establece el número de puntos de
    * control que se capturarán en este frame
    *
    * \param[in] Number Número de puntos que serán capturados en el frame
    */
   void SetNumberOfNumberOfPointsToTrack(int Number);

   /*!
    * \brief SetTrackedPoints Establece un punto de control.
    * \param PointId Posición del punto
    * \param x Coordenada del punto en x
    * \param y Coordenada del punto en y
    */
   void SetTrackedPoints(int PointId, int x, int y);

   /*!
    * \brief AddTrackedPoint agrega un punto de control
    * \param x Coordenada del punto en x
    * \param y Coordenada del punto en y
    * \todo Cambiar NumberOfTRegisteredPoints al agregar
    * \todo Validar que se no se agreguen más puntos de los que se debe
    */
   void AddTrackedPoint(int x, int y);
};

} // namespace RatWalkCore
#endif // RATWALKFRAMEOBJECT_H
