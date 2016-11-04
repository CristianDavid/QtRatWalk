#include <algorithm>
#include "RatWalkTrackerVideoObject.h"

int RatWalkTrackerVideoObject::OpenVideoFile(char VideoFileName[]){
   videoObject.open(VideoFileName);
   if(!videoObject.isOpened())  // check if we succeeded
      return 0;
   Fps = videoObject.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
   NumberOfFrames = videoObject.get(CV_CAP_PROP_FRAME_COUNT); //get the number of frames in the video
   Width = videoObject.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of the video
   Height = videoObject.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of the video

   //build RatWalkFrame objects for each frame
   FrameProperties=new RatWalkFrameObject[NumberOfFrames];
   //Initialize the tracking points objects for each frame
   for (int Frame=0;Frame<NumberOfFrames;Frame++)
      FrameProperties[Frame].SetNumberOfNumberOfPointsToTrack(NUMBEROFPOINTSTOTRACK);

   //Read the first frame
   bool bSuccess = videoObject.read(CurrentFrameData);
   if (!bSuccess) //if not success, break loop
   {
      std::cout << "Cannot read the frame from video file" << std::endl;
      return 0;
   }
   cvtColor(CurrentFrameData,CurrentHSVFrameData,CV_BGR2HSV);
   return 1;
}

int RatWalkTrackerVideoObject::OpenLiveStream(int DeviceNumber){
  videoObject.open(DeviceNumber);
  return 1;
}

int RatWalkTrackerVideoObject::GetNextFrame(){
  CurrentFrame++;
  if (CurrentFrame>=NumberOfFrames)
      CurrentFrame=NumberOfFrames-1;
 // bool bSuccess = VideoObject.read(CurrentFrameData);
  videoObject.set(CV_CAP_PROP_POS_FRAMES, CurrentFrame);
  bool bSuccess = videoObject.read(CurrentFrameData);
  if (!bSuccess) //if not success, break loop
  {
      std::cout << "Cannot read the frame from video file" << std::endl;
      return 0;
  }
  cvtColor(CurrentFrameData,CurrentHSVFrameData,CV_BGR2HSV);
  return 1;
}

int RatWalkTrackerVideoObject::GetPreviousFrame(){
  CurrentFrame--;
  if (CurrentFrame<0)
      CurrentFrame=0;
  videoObject.set(CV_CAP_PROP_POS_FRAMES, CurrentFrame);
  bool bSuccess = videoObject.read(CurrentFrameData);
  if (!bSuccess) //if not success, break loop
  {
      std::cout << "Cannot read the frame from video file" << std::endl;
      return 0;
  }
  cvtColor(CurrentFrameData,CurrentHSVFrameData,CV_BGR2HSV);
  return 1;
}

int RatWalkTrackerVideoObject::GetFrameNumber(double FrameNumber){
  CurrentFrame=FrameNumber;
  if (CurrentFrame<0)
      CurrentFrame=0;
  if (CurrentFrame>=NumberOfFrames)
      CurrentFrame=NumberOfFrames-1;
  videoObject.set(CV_CAP_PROP_POS_FRAMES, CurrentFrame);
  bool bSuccess = videoObject.read(CurrentFrameData);
  if (!bSuccess) //if not success, break loop
  {
      std::cout << "Cannot read the frame from video file" << std::endl;
      return 0;
  }
  cvtColor(CurrentFrameData,CurrentHSVFrameData,CV_BGR2HSV);
  return 1;
}


//Function to show the skeleton in the current frame
void RatWalkTrackerVideoObject::ShowSkeletonInCurrentFrame(){
  cv::Mat CurrentFrameWithSkeleton=CurrentFrameData.clone();
  int NumberOfPointsToShow=FrameProperties[CurrentFrame].NumberOfTRegisteredPoints;
  for (int i=1;i<NumberOfPointsToShow;i++){
      int x1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].x);
      int y1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].y);
      int x2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i].x);
      int y2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i].y);
      cv::circle(CurrentFrameWithSkeleton, cv::Point(x1,y1),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
      cv::circle(CurrentFrameWithSkeleton, cv::Point(x2,y2),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
      cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1), cv::Point(x2,y2), cv::Scalar(0,200,0), 2, CV_AA );

      //Draw the angle
      cv::line(CurrentFrameWithSkeleton, cv::Point(x1-20,y1), cv::Point(x1+20,y1), cv::Scalar(255,255,0), .5, CV_AA );
      cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1-20), cv::Point(x1,y1+20), cv::Scalar(255,255,0), .5, CV_AA );
      std::ostringstream strs;
      strs << FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].Theta;
      std::string Angle=strs.str();
      cv::putText(CurrentFrameWithSkeleton, Angle, cv::Point2f(x1,y1), cv::FONT_HERSHEY_SIMPLEX, .5,  cv::Scalar(255,255,255,255),1);

      if (i==4){
          //Show the pendular movement
           x1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[0].x);
           y1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[0].y);
           x2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[3].x);
           y2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[3].y);
          cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1), cv::Point(x2,y2), cv::Scalar(0,200,200), 2, CV_AA );

          //Draw the angle
          std::ostringstream strs;
          strs << FrameProperties[CurrentFrame].TrackedPointsInFrame[4].Theta;
          std::string Angle=strs.str();
          cv::putText(CurrentFrameWithSkeleton, Angle, cv::Point2f(x1,y1+20), cv::FONT_HERSHEY_SIMPLEX, .8,  cv::Scalar(0,255,255,255),1);


      }
  }




  char str[200];
  std::sprintf(str,"Frame %d",CurrentFrame);
  cv::putText(CurrentFrameWithSkeleton, str, cv::Point2f(10,20), cv::FONT_HERSHEY_SIMPLEX, .5,  cv::Scalar(0,0,0,255),2);

  //imshow("RatWalkVideo", CurrentFrameWithSkeleton);
}


void RatWalkTrackerVideoObject::ShowFrameWithTrackingPoints(){
  cv::Mat FrameWithRectangle = CurrentFrameData.clone();
  for (int PointToShow=0;PointToShow<FrameProperties[CurrentFrame].NumberOfTRegisteredPoints;PointToShow++)
      cv::circle(FrameWithRectangle, cv::Point((int)round(FrameProperties[CurrentFrame].TrackedPointsInFrame[PointToShow].x),(int)round(FrameProperties[CurrentFrame].TrackedPointsInFrame[PointToShow].y)),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
  cv::imshow("RatWalkVideo", FrameWithRectangle);

}

cv::Mat RatWalkTrackerVideoObject::getFrameWithTrackingPoints() {
   cv::Mat FrameWithRectangle = CurrentFrameData.clone();
   for (int PointToShow=0;PointToShow<FrameProperties[CurrentFrame].NumberOfTRegisteredPoints;PointToShow++)
       cv::circle(FrameWithRectangle, cv::Point((int)round(FrameProperties[CurrentFrame].TrackedPointsInFrame[PointToShow].x),(int)round(FrameProperties[CurrentFrame].TrackedPointsInFrame[PointToShow].y)),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
   return FrameWithRectangle;
}

cv::Mat RatWalkTrackerVideoObject::getFrameWithSkeleton() {
   cv::Mat CurrentFrameWithSkeleton=CurrentFrameData.clone();
   int NumberOfPointsToShow=FrameProperties[CurrentFrame].NumberOfTRegisteredPoints;
   for (int i=1;i<NumberOfPointsToShow;i++){
       int x1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].x);
       int y1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].y);
       int x2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i].x);
       int y2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[i].y);
       cv::circle(CurrentFrameWithSkeleton, cv::Point(x1,y1),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
       circle(CurrentFrameWithSkeleton, cv::Point(x2,y2),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);
       cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1), cv::Point(x2,y2), cv::Scalar(0,200,0), 2, CV_AA );

       //Draw the angle
       cv::line(CurrentFrameWithSkeleton, cv::Point(x1-20,y1), cv::Point(x1+20,y1), cv::Scalar(255,255,0), .5, CV_AA );
       cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1-20), cv::Point(x1,y1+20), cv::Scalar(255,255,0), .5, CV_AA );
       std::ostringstream strs;
       strs << FrameProperties[CurrentFrame].TrackedPointsInFrame[i-1].Theta;
       std::string Angle=strs.str();
       cv::putText(CurrentFrameWithSkeleton, Angle, cv::Point2f(x1,y1), cv::FONT_HERSHEY_SIMPLEX, .5,  cv::Scalar(255,255,255,255),1);

       if (i==4){
           //Show the pendular movement
            x1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[0].x);
            y1=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[0].y);
            x2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[3].x);
            y2=round(FrameProperties[CurrentFrame].TrackedPointsInFrame[3].y);
           cv::line(CurrentFrameWithSkeleton, cv::Point(x1,y1), cv::Point(x2,y2), cv::Scalar(0,200,200), 2, CV_AA );

           //Draw the angle
           std::ostringstream strs;
           strs << FrameProperties[CurrentFrame].TrackedPointsInFrame[4].Theta;
           std::string Angle=strs.str();
           cv::putText(CurrentFrameWithSkeleton, Angle, cv::Point2f(x1,y1+20), cv::FONT_HERSHEY_SIMPLEX, .8,  cv::Scalar(0,255,255,255),1);


       }
   }
   return CurrentFrameWithSkeleton;
}

cv::Mat RatWalkTrackerVideoObject::getZoomedRegion(int x, int y, int halfWindowSize) {
   cv::Mat frameWithRectangle = getFrameWithTrackingPoints();
   cv::Mat SegmentedZoomedImageRegion;
   cv::Point WindowPoint1, WindowPoint2;
   cv::Rect BoundingBox;

   WindowPoint1.x=x-halfWindowSize;
   if (WindowPoint1.x<0)
       WindowPoint1.x=0;
   WindowPoint1.y=y-halfWindowSize;
   if (WindowPoint1.y<0)
       WindowPoint1.y=0;
   WindowPoint2.x=x+halfWindowSize;
   if (WindowPoint2.x>Width-1)
       WindowPoint2.x=Width-1;
   WindowPoint2.y=y+halfWindowSize;
   if (WindowPoint2.y>Height-1)
       WindowPoint2.y=Height-1;

   BoundingBox = cv::Rect(WindowPoint1.x,WindowPoint1.y,WindowPoint2.x-WindowPoint1.x,WindowPoint2.y-WindowPoint1.y);

   cv::circle(frameWithRectangle, cv::Point((int)round(x),(int)round(y)),1, cv::Scalar(255,0,0),CV_FILLED, 1,0);

   cv::Mat SegmentedRegion= frameWithRectangle(BoundingBox);
   cv::Size dsize = cv::Size(500,500);
   resize(SegmentedRegion, SegmentedZoomedImageRegion, dsize, cv::INTER_NEAREST);
   return SegmentedZoomedImageRegion;
}

void RatWalkTrackerVideoObject::SelectPoint(int x, int y, int HalfWindowSize, int PointId){

  std::cout<<"\n CoorX="<<x<<" CoorY="<<y;

  cv::Rect BoundingBoxData; /* bounding box */
  cv::Mat FrameClone = CurrentFrameData.clone();
  cv::Mat HSVFrameClone=CurrentHSVFrameData.clone();
  cv::Mat FrameWithRectangle = CurrentFrameData.clone();

  cv::Mat ImageRegion;
  cv::Mat ZoomedRegion;
  cv::Mat SegmentedZoomedImageRegion;
  cv::Point WindowPoint1, WindowPoint2;
  cv::Point RegionPoint1, RegionPoint2;

  WindowPoint1.x=x-HalfWindowSize;
  if (WindowPoint1.x<0)
      WindowPoint1.x=0;
  WindowPoint1.y=y-HalfWindowSize;
  if (WindowPoint1.y<0)
      WindowPoint1.y=0;
  WindowPoint2.x=x+HalfWindowSize;
  if (WindowPoint2.x>Width-1)
      WindowPoint2.x=Width-1;
  WindowPoint2.y=y+HalfWindowSize;
  if (WindowPoint2.y>Height-1)
      WindowPoint2.y=Height-1;

  std::cout<<"\n WindowFrom ("<<WindowPoint1.x<<","<<WindowPoint1.y<<") to "<<"("<<WindowPoint2.x<<","<<WindowPoint2.y<<")";

  //Draw the rectangoe in the main image
  cv::rectangle(FrameWithRectangle, WindowPoint1, WindowPoint2, CV_RGB(255, 0, 0), 3, 8, 0);


  //Get a rectange around the selection point
  BoundingBoxData = cv::Rect(WindowPoint1.x,WindowPoint1.y,WindowPoint2.x-WindowPoint1.x,WindowPoint2.y-WindowPoint1.y);

  //Auxiliar image to show the segmented region in color
  cv::Mat SegmentedRegion= FrameClone(BoundingBoxData);



  cv::circle(SegmentedRegion, cv::Point(HalfWindowSize+1,HalfWindowSize+1),1, cv::Scalar(0,0,255),CV_FILLED, 1,0);


      cv::circle(FrameWithRectangle, cv::Point((int)round(x),(int)round(y)),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);



  std::cout << "Control Point registered" <<std::endl;
     cv::Size dsize = cv::Size(500,500);
  //  resize(ImageRegion, ZoomedRegion, dsize);
  cv::resize(SegmentedRegion, SegmentedZoomedImageRegion, dsize,cv::INTER_NEAREST);


  //imshow("ZoomedRegion", SegmentedZoomedImageRegion);
  //imshow("RatWalkVideo", FrameWithRectangle);

  RatWalkFrameObject &currentFrame = FrameProperties[CurrentFrame];
  currentFrame.TrackedPointsInFrame[PointId].x=x;
  currentFrame.TrackedPointsInFrame[PointId].y=y;
  if (currentFrame.NumberOfTRegisteredPoints < 5) {
      currentFrame.NumberOfTRegisteredPoints++;
  }

  if (PointId>0){
      double xa=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId-1].x;
      double ya=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId-1].y;
      double xb=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].x;
      double yb=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].y;

      double Angle=180*atan2((yb-ya),(xb-xa))/3.1416;
      FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId-1].Theta=Angle;

  }

  if (PointId==4){
      double xa=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[0].x;
      double ya=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[0].y;
      double xb=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[3].x;
      double yb=(double) FrameProperties[CurrentFrame].TrackedPointsInFrame[3].y;

      double Angle=180*atan2((yb-ya),(xb-xa))/3.1416;
      FrameProperties[CurrentFrame].TrackedPointsInFrame[4].Theta=Angle;

  }



}


void RatWalkTrackerVideoObject::SegmentRegionFromSeed(int x, int y, int HalfWindowSize, int PointId){

  std::cout<<"\n CoorX="<<x<<" CoorY="<<y;

  cv::Rect BoundingBoxData; /* bounding box */
  cv::Mat FrameClone = CurrentFrameData.clone();
  cv::Mat HSVFrameClone=CurrentHSVFrameData.clone();
  cv::Mat FrameWithRectangle = CurrentFrameData.clone();
  cv::Mat ImageRegion;
  cv::Mat ZoomedRegion;
  cv::Mat SegmentedZoomedImageRegion;
  cv::Point WindowPoint1, WindowPoint2;
  cv::Point RegionPoint1, RegionPoint2;

  WindowPoint1.x=x-HalfWindowSize;
  if (WindowPoint1.x<0)
      WindowPoint1.x=0;
  WindowPoint1.y=y-HalfWindowSize;
  if (WindowPoint1.y<0)
      WindowPoint1.y=0;
  WindowPoint2.x=x+HalfWindowSize;
  if (WindowPoint2.x>Width-1)
      WindowPoint2.x=Width-1;
  WindowPoint2.y=y+HalfWindowSize;
  if (WindowPoint2.y>Height-1)
      WindowPoint2.y=Height-1;

  std::cout<<"\n WindowFrom ("<<WindowPoint1.x<<","<<WindowPoint1.y<<") to "<<"("<<WindowPoint2.x<<","<<WindowPoint2.y<<")";

  //Draw the rectangoe in the main image
  rectangle(FrameWithRectangle, WindowPoint1, WindowPoint2, CV_RGB(255, 0, 0), 3, 8, 0);


  //Get a rectange around the selection point
  BoundingBoxData = cv::Rect(WindowPoint1.x,WindowPoint1.y,WindowPoint2.x-WindowPoint1.x,WindowPoint2.y-WindowPoint1.y);

  //Get the region of interest in HSV space
  ImageRegion = HSVFrameClone(BoundingBoxData);
  //Size of the zoomed region to show
  cv::Size dsize = cv::Size(500,500);
  //Auxiliar image to show the segmented region in color
  cv::Mat SegmentedRegion= FrameClone(BoundingBoxData);



  //Set the pixel values as features for the clustering
  cv::Mat HSVPixelsFeatureVector(ImageRegion.rows*ImageRegion.cols,3,CV_32F);
  for( int y = 0; y < ImageRegion.rows; y++ )
      for( int x = 0; x < ImageRegion.cols; x++ )
          for( int z = 0; z < 3; z++)
              HSVPixelsFeatureVector.at<float>(y + x*ImageRegion.rows, z) = ImageRegion.at<cv::Vec3b>(y,x)[z];

  //Perform the Kmeans Clustering
  int clusterCount = 5;
  cv::Mat labels;
  int attempts = 5;
  cv::Mat centers;
  kmeans(HSVPixelsFeatureVector, clusterCount, labels, cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers );

  /*

  //Auxiliar code to check clustering resutl
  cv::Mat new_image( ImageRegion.size(), ImageRegion.type() );
  for( int y = 0; y < ImageRegion.rows; y++ )
      for( int x = 0; x < ImageRegion.cols; x++ )
      {
          int cluster_idx = labels.at<int>(y + x*ImageRegion.rows,0);
          new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
          new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
          new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
      }
resize(new_image, SegmentedZoomedImageRegion, dsize);

  imshow( "ZoomedRegion", SegmentedZoomedImageRegion );

*/




  cv::Mat SegmentedRegionMask; // segmentation result
  SegmentedRegionMask = cv::Mat::zeros(ImageRegion.rows,ImageRegion.cols,CV_8UC1);

  cv::Vec3b SampleColor;
  //SampleColor.val[0]=ImageRegion.at<cv::Vec3b>(Point(HalfWindowSize+1,HalfWindowSize+1))[0];
  //SampleColor.val[1]=ImageRegion.at<cv::Vec3b>(Point(HalfWindowSize+1,HalfWindowSize+1))[1];
  //SampleColor.val[2]=ImageRegion.at<cv::Vec3b>(Point(HalfWindowSize+1,HalfWindowSize+1))[2];

  //Get the cluster index of the pixel in the center of the region
  int RegionCenterClusterIDX=labels.at<int>((HalfWindowSize+1) + (HalfWindowSize+1)*ImageRegion.rows,0);
  //Save the centroid values of the pixel
  SampleColor.val[0]=centers.at<float>(RegionCenterClusterIDX, 0);
  SampleColor.val[1]=centers.at<float>(RegionCenterClusterIDX, 1);
  SampleColor.val[2]=centers.at<float>(RegionCenterClusterIDX, 2);


  int ColTol=15;

  //For the computation of the region Centroid
  float MeanX=0,MeanY=0;
  int NumberOfPixelsInRegion=0;

  for( int y = 0; y < ImageRegion.rows; y++ ){
      for( int x = 0; x < ImageRegion.cols; x++ )
      {
          int cluster_idx = labels.at<int>(y + x*ImageRegion.rows,0);
          //if (abs(centers.at<float>(cluster_idx, 0)-SampleColor.val[0])<ColTol)
          if (cluster_idx==RegionCenterClusterIDX)
           {
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[0] = 0;
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[1] = 255;
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[2] = 100;
              MeanX+=(float)x;
              MeanY+=(float)y;
              NumberOfPixelsInRegion++;
          }
      }
  }

  //Compute the centroid using means
  MeanX=MeanX/(float)NumberOfPixelsInRegion;
  MeanY=MeanY/(float)NumberOfPixelsInRegion;

  circle(SegmentedRegion, cv::Point((int)round(MeanX),(int)round(MeanY)),1, cv::Scalar(0,0,255),CV_FILLED, 1,0);


  //Convert coordinates to the big image
  MeanX+=(float)WindowPoint1.x;
  MeanY+=(float)WindowPoint1.y;


  circle(FrameWithRectangle, cv::Point((int)round(MeanX),(int)round(MeanY)),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);



  std::cout << "Control Point Segmented" << std::endl;



  //  resize(ImageRegion, ZoomedRegion, dsize);
  resize(SegmentedRegion, SegmentedZoomedImageRegion, dsize, cv::INTER_NEAREST);


  imshow("ZoomedRegion", SegmentedZoomedImageRegion);
  imshow("RatWalkVideo", FrameWithRectangle);

  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].x=MeanX;
  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].y=MeanY;
  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].MarkerColor=SampleColor;
}

int RatWalkTrackerVideoObject::SegmentRegionFromColorStatistics(int x, int y, int HalfWindowSize, int PointId, cv::Vec3b SampleColor){
          std::cout<<"\n CoorX="<<x<<" CoorY="<<y;

  cv::Rect BoundingBoxData; /* bounding box */
  cv::Mat FrameClone = CurrentFrameData.clone();
  cv::Mat HSVFrameClone=CurrentHSVFrameData.clone();
  cv::Mat FrameWithRectangle = CurrentFrameData.clone();
  cv::Mat ImageRegion;
  cv::Mat ZoomedRegion;
  cv::Mat SegmentedZoomedImageRegion;
  cv::Point WindowPoint1, WindowPoint2;
  cv::Point RegionPoint1, RegionPoint2;

  WindowPoint1.x=x-HalfWindowSize;
  if (WindowPoint1.x<0)
      WindowPoint1.x=0;
  WindowPoint1.y=y-HalfWindowSize;
  if (WindowPoint1.y<0)
      WindowPoint1.y=0;
  WindowPoint2.x=x+HalfWindowSize;
  if (WindowPoint2.x>Width-1)
      WindowPoint2.x=Width-1;
  WindowPoint2.y=y+HalfWindowSize;
  if (WindowPoint2.y>Height-1)
      WindowPoint2.y=Height-1;

  //Draw the rectangoe in the main image
  rectangle(FrameWithRectangle, WindowPoint1, WindowPoint2, CV_RGB(255, 0, 0), 3, 8, 0);

  std::cout<<"\n WindowFrom ("<<WindowPoint1.x<<","<<WindowPoint1.y<<") to "<<"("<<WindowPoint2.x<<","<<WindowPoint2.y<<")";



  //Get a rectange around the selection point
  BoundingBoxData = cv::Rect(WindowPoint1.x,WindowPoint1.y,WindowPoint2.x-WindowPoint1.x,WindowPoint2.y-WindowPoint1.y);
  //Get the region of interest in HSV space
  ImageRegion = HSVFrameClone(BoundingBoxData);
  //Size of the zoomed region to show
  cv::Size dsize = cv::Size(500,500);
  //Auxiliar image to show the segmented region in color
  cv::Mat SegmentedRegion= FrameClone(BoundingBoxData);


  //Set the pixel values as features for the clustering
  cv::Mat HSVPixelsFeatureVector(ImageRegion.rows*ImageRegion.cols,3,CV_32F);
  for( int y = 0; y < ImageRegion.rows; y++ )
      for( int x = 0; x < ImageRegion.cols; x++ )
          for( int z = 0; z < 3; z++)
              HSVPixelsFeatureVector.at<float>(y + x*ImageRegion.rows, z) = ImageRegion.at<cv::Vec3b>(y,x)[z];

  //Perform the Kmeans Clustering
  int clusterCount = 5;
  cv::Mat labels;
  int attempts = 5;
  cv::Mat centers;
  kmeans(HSVPixelsFeatureVector, clusterCount, labels, cv::TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, cv::KMEANS_PP_CENTERS, centers );

  /*

   //Auxiliar code to check clustering resutl
   cv::Mat new_image( ImageRegion.size(), ImageRegion.type() );
   for( int y = 0; y < ImageRegion.rows; y++ )
   for( int x = 0; x < ImageRegion.cols; x++ )
   {
   int cluster_idx = labels.at<int>(y + x*ImageRegion.rows,0);
   new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
   new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
   new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
   }
   resize(new_image, SegmentedZoomedImageRegion, dsize);

   imshow( "ZoomedRegion", SegmentedZoomedImageRegion );
   */




  cv::Mat SegmentedRegionMask; // segmentation result
  SegmentedRegionMask = cv::Mat::zeros(ImageRegion.rows,ImageRegion.cols,CV_8UC1);


  //Find the cluster index closer to the last known color
  cv::Vec3b CentroidColor;
  int RegionCenterClusterIDX=0;
  CentroidColor.val[0]=centers.at<float>(RegionCenterClusterIDX, 0);
  CentroidColor.val[1]=centers.at<float>(RegionCenterClusterIDX, 1);
  CentroidColor.val[2]=centers.at<float>(RegionCenterClusterIDX, 2);
  float ColourDistanceOld=sqrt( pow((float)SampleColor.val[0]-CentroidColor.val[0],2)+pow((float)SampleColor.val[1]-CentroidColor.val[1],2)+pow((float)SampleColor.val[2]-CentroidColor.val[2],2)     );

  for (int i=1;i<clusterCount;i++){
          cv::Vec3b NewCentroidColor;
          NewCentroidColor.val[0]=centers.at<float>(i, 0);
          NewCentroidColor.val[1]=centers.at<float>(i, 1);
          NewCentroidColor.val[2]=centers.at<float>(i, 2);
          float ColourDistanceNew=sqrt( pow((float)SampleColor.val[0]-NewCentroidColor.val[0],2)+pow((float)SampleColor.val[1]-NewCentroidColor.val[1],2)+pow((float)SampleColor.val[2]-NewCentroidColor.val[2],2));
          if (ColourDistanceNew<ColourDistanceOld) {
              RegionCenterClusterIDX=i;
              CentroidColor.val[0]=NewCentroidColor.val[0];
              CentroidColor.val[1]=NewCentroidColor.val[1];
              CentroidColor.val[2]=NewCentroidColor.val[2];
              ColourDistanceOld=ColourDistanceNew;
          }

  }


  int ColTol=15;

  //For the computation of the region Centroid
  float MeanX=0,MeanY=0;
  int NumberOfPixelsInRegion=0;

  for( int y = 0; y < ImageRegion.rows; y++ ){
      for( int x = 0; x < ImageRegion.cols; x++ )
      {
          int cluster_idx = labels.at<int>(y + x*ImageRegion.rows,0);
          //if (abs(centers.at<float>(cluster_idx, 0)-SampleColor.val[0])<ColTol)
          if (cluster_idx==RegionCenterClusterIDX)
          {
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[0] = 0;
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[1] = 255;
              SegmentedRegion.at<cv::Vec3b>(cv::Point(x,y))[2] = 100;
              MeanX+=(float)x;
              MeanY+=(float)y;
              NumberOfPixelsInRegion++;
          }
      }
  }

  if (NumberOfPixelsInRegion<1)
      return 0;

  //Compute the centroid using means
  MeanX=MeanX/(float)NumberOfPixelsInRegion;
  MeanY=MeanY/(float)NumberOfPixelsInRegion;


  circle(SegmentedRegion, cv::Point((int)round(MeanX),(int)round(MeanY)),1, cv::Scalar(0,0,255),CV_FILLED, 1,0);
  resize(SegmentedRegion, SegmentedZoomedImageRegion, dsize);
  imshow("ZoomedRegion", SegmentedZoomedImageRegion);
  //int k=waitKey(0);

  //Convert coordinates to the big image
  MeanX+=(float)WindowPoint1.x;
  MeanY+=(float)WindowPoint1.y;


  circle(FrameWithRectangle, cv::Point((int)round(MeanX),(int)round(MeanY)),3, cv::Scalar(0,0,255),CV_FILLED, 1,0);



  std::cout << "Control Point Segmented" << std::endl;




  imshow("RatWalkVideo", FrameWithRectangle);
//   int kk=waitKey(0);

  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].x=MeanX;
  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].y=MeanY;
  FrameProperties[CurrentFrame].TrackedPointsInFrame[PointId].MarkerColor=CentroidColor;
  return 1;

}

int RatWalkTrackerVideoObject::RelaseVideo(){
  videoObject.release();
  return 1;
}
