#ifndef RATWALKFRAMEOBJECT_H
#define RATWALKFRAMEOBJECT_H

#include <vector>
#include "RatWalkCore/ControlPoint.h"

namespace RatWalkCore {

/*!
 * \brief This class represents a frame of a video with the pointes captured
 *        on it.
 *
 * \see RatWalkCore::ControlPoint
 */
class Frame {
public:
   bool IsSet=0;
   int FrameNumber;
   std::vector<ControlPoint> TrackedPointsInFrame;
   int NumberOfTRegisteredPoints=0;
   int NumberOfPointsToTrack;

   /*!
    * \brief SetNumberOfNumberOfPointsToTrack sets the number of control points
    *        that will be captured on this frame.
    *
    * \param[in] Number Number of points to be captured on this frame
    */
   void SetNumberOfNumberOfPointsToTrack(int Number);

   /*!
    * \brief SetTrackedPoints Sets a ControlPoint.
    * \param[in] PointId Id of the point that will be set
    * \param[in] x X coordinate of the point
    * \param[in] y Y coordinate of the point
    */
   void SetTrackedPoints(int PointId, int x, int y);

   /*!
    * \brief AddTrackedPoint adds a ControlPoint
    * \param x X coordinate of the point
    * \param y Y coordinate of the point
    */
   void AddTrackedPoint(int x, int y);
};

} // namespace RatWalkCore
#endif // RATWALKFRAMEOBJECT_H
