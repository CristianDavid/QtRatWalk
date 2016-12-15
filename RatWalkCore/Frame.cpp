#include "RatWalkCore/Frame.h"

#include <vector>
#include "RatWalkCore/ControlPoint.h"

namespace RatWalkCore {

void Frame::SetNumberOfNumberOfPointsToTrack(int Number) {
   NumberOfPointsToTrack = Number;
   TrackedPointsInFrame  = std::vector<ControlPoint>(NumberOfPointsToTrack);
}

void Frame::SetTrackedPoints(int PointId, int x, int y){
   TrackedPointsInFrame[PointId].CoorX=x;
   TrackedPointsInFrame[PointId].CoorY=y;
}

void Frame::AddTrackedPoint(int x, int y) {
   ControlPoint point = {x, y, 0, 0, 0.0, 0.0, cv::Vec3b()};
   TrackedPointsInFrame.push_back(point);
}

} // namespace RatWalkCore
