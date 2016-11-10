#include "RatWalkFrameObject.h"

void RatWalkFrameObject::SetNumberOfNumberOfPointsToTrack(int Number){
   NumberOfPointsToTrack=Number;
   TrackedPointsInFrame=new RatWalkControlPoint[NumberOfPointsToTrack];
}

void RatWalkFrameObject::SetTrackedPoints(int PointId, int x, int y){
   TrackedPointsInFrame[PointId].CoorX=x;
   TrackedPointsInFrame[PointId].CoorY=y;
}
