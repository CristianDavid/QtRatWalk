#ifndef RATWALKTRACKER_H
#define RATWALKTRACKER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "RatWalkTrackerVideoObject.h"

#define HalfWindowSize 9
#define NpointsToTrack 5

class RatWalkTracker {
 public:
   RatWalkTracker(const char *fileName);
   void mouseHandlerForInitialTrackingPoints(int event, int x, int y, int flags);
   void addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight);
   void setPointOnCurrentFrame(int pointId, int x, int y,
                               int frameWidth, int frameHeight);
   int getClosestPointID(int x, int y, int frameWidth, int frameHeight,
                         double minDistance = 1.0);
   int SelectFrameForAddingTrackingPoints();
   void on_trackbar(int Position);
   int ratWalkMain();
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
   RatWalkTrackerVideoObject VideoToAnalyze[3];
   std::vector<std::string> videoNames;
   int CurrentVideoAnalyzed = 0;
   int PointID = 0;
   int NumberOfVideos;
   char OutputFileName[400];
};

#endif // RATWALKTRACKER_H
