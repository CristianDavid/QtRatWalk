#ifndef RATWALKFRAMEOBJECT_H
#define RATWALKFRAMEOBJECT_H

#include "RatWalkControlPoint.h"

class RatWalkFrameObject{
public:
   bool IsSet=0;
   int FrameNumber;
   RatWalkControlPoint *TrackedPointsInFrame;
   int NumberOfTRegisteredPoints=0;
   int NumberOfPointsToTrack;

   void SetNumberOfNumberOfPointsToTrack(int Number);
   void SetTrackedPoints(int PointId, int x, int y);
};
#endif // RATWALKFRAMEOBJECT_H
