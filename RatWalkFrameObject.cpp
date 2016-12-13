#include "RatWalkFrameObject.h"

#include <vector>
#include "RatWalkControlPoint.h"

void RatWalkFrameObject::SetNumberOfNumberOfPointsToTrack(int Number) {
   NumberOfPointsToTrack = Number;
   TrackedPointsInFrame  = std::vector<RatWalkControlPoint>(NumberOfPointsToTrack);
}

void RatWalkFrameObject::SetTrackedPoints(int PointId, int x, int y){
   TrackedPointsInFrame[PointId].CoorX=x;
   TrackedPointsInFrame[PointId].CoorY=y;
}

void RatWalkFrameObject::AddTrackedPoint(int x, int y) {
   RatWalkControlPoint point = {x, y, 0, 0, 0.0, 0.0, cv::Vec3b()};
   TrackedPointsInFrame.push_back(point);
}
