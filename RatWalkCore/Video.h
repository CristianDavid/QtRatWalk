#ifndef RATWALKTRACKERVIDEOOBJECT_H
#define RATWALKTRACKERVIDEOOBJECT_H

#include "RatWalkCore/Frame.h"

namespace RatWalkCore {

/*!
 * \brief Datos de un vídeo para analizar
 * \todo Entender mejor la clase
 */
class Video {
private:
   cv::VideoCapture videoObject;
public:
   int NumberOfFrames;
   int Width;
   int Height;
   double Fps;
   cv::Mat CurrentFrameData;
   cv::Mat CurrentHSVFrameData;
   int CurrentFrame=0;
   int NumberOfTrackingPoints=0;
   Frame *FrameProperties;
   bool StartTrackingPointProcessing=false;

   int OpenVideoFile(char VideoFileName[]);
   int OpenLiveStream(int DeviceNumber);
   int GetNextFrame();
   int GetPreviousFrame();
   int GetFrameNumber(double FrameNumber);
    //Function to show the skeleton in the current frame
   /*!
    * \brief ShowSkeletonInCurrentFrame
    * \todo creo que estas funciones "Show" deberían irse
    */
   void ShowSkeletonInCurrentFrame();
   void ShowFrameWithTrackingPoints();
   //Function to show the skeleton in the current frame
   void ShowSkeletonInCurrentTransformedFrame(cv::Mat TransformedFrame);
   cv::Mat getFrameWithTrackingPoints();
   cv::Mat getFrameWithSkeleton();
   cv::Mat getZoomedRegion(int x, int y, int halfWindowSize);
   void SelectPoint(int x, int y, int HalfWindowSize, int PointId, int CurrentVideoAnalyzed);
   void SegmentRegionFromSeed(int x, int y, int HalfWindowSize, int PointId);
   int SegmentRegionFromColorStatistics(int x, int y, int HalfWindowSize,
                                         int PointId, cv::Vec3b SampleColor);
   int RelaseVideo();
};

} // namespace RatWalkCore

#endif // RATWALKTRACKERVIDEOOBJECT_H
