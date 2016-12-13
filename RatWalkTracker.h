#ifndef RATWALKTRACKER_H
#define RATWALKTRACKER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "RatWalkTrackerVideoObject.h"
#include "RatWalkFile.h"

#define HalfWindowSize 9
#define NpointsToTrack 5

class RatWalkTracker {
 public:
   RatWalkTracker(const char *fileName);
   void mouseHandlerForInitialTrackingPoints(int event, int x, int y, int flags);
   void addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight);
   void setPointOnCurrentFrame(int pointId, int x, int y,
                               int frameWidth, int frameHeight);
   void deletePointOnCurrentFrame(int pointId);
   int getClosestPointID(int x, int y, int frameWidth, int frameHeight,
                         double minDistance = 1.0);
   int SelectFrameForAddingTrackingPoints();
   void on_trackbar(int Position);
   void nextFrame();
   void prevFrame();
   void guardar();
   void traeEsqueleto();
   cv::Mat getFrameWithRectangle();
   cv::Mat getFrameWithSkeleton();
   cv::Mat getZoomedRegion(int x, int y, int frameWidth, int frameHeight);
   const RatWalkTrackerVideoObject &getCurrentVideoAnalyzed();
   const std::vector<std::string> &getVideoNames();
   void setCurrentVideo(int index);

 private:
   cv::Mat TargetImage, TargetImageGray,
           Image1,     Image2,       Image3,
           ImageLeft,  ImageMiddle,  ImageRight,
           ImageLeftG, ImageMiddleG, ImageRightG, TargetImageGrayG;
   RatWalkTrackerVideoObject VideoToAnalyze[3];
   int CurrentVideoAnalyzed = 0;
   int PointID = 0;
   RatWalkFile ratFile;
};

#endif // RATWALKTRACKER_H
