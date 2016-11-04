#include <algorithm>
#include <stdio.h>
#include <cstdlib>
#include "opencv2/opencv.hpp"
#include <unistd.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include "RatWalkTrackerVideoObject.h"
#include "RatWalkTracker.h"
#include "Points.h"

using namespace cv;
using namespace std;

/*####################################################
 Mouse interaction´s callback function
 #####################################################*/

void mouseHandlerForInitialTrackingPoints(int event, int x, int y, int flags, void* param) {
   RatWalkTracker *obj = static_cast<RatWalkTracker *>(param);
   obj->mouseHandlerForInitialTrackingPoints(event, x, y, flags);
}

void on_trackbar(int Position, void *param) {
   RatWalkTracker *obj = static_cast<RatWalkTracker *>(param);
   obj->on_trackbar(Position);
}

RatWalkTracker::RatWalkTracker(const char *fileName) {
   NumberOfVideos=0;
   CurrentVideoAnalyzed=0;
   string line[10];
   ifstream myfile (fileName);
   if (myfile.is_open()) {

       while ( getline (myfile,line[NumberOfVideos]) ) {
           cout << line[NumberOfVideos] << '\n';
           NumberOfVideos++;
       }
       myfile.close();
   }

   //Open the video files
   for (int i = 0; i < NumberOfVideos; i++) {
      const char *FileNameToRead=line[i].c_str();
      videoNames.push_back(FileNameToRead);
      if (!VideoToAnalyze[i].OpenVideoFile((char *)FileNameToRead)) {
         cout<<"Video File "<<FileNameToRead <<" Could not be opened";
         return;
//         return 0;
      }
   }

   //Create the outputFile
   string delimiter = "+";
   string token =  line[CurrentVideoAnalyzed].substr(0,  line[CurrentVideoAnalyzed].find(delimiter));

   sprintf(OutputFileName, "%s.csv",token.c_str());

   //Try to read the previously annotated things
   string lineToParse;
   ifstream PreviouslyAnnotatedFile (OutputFileName);
   string delimiterRead = ",";
   if (PreviouslyAnnotatedFile.is_open()) {
       //Get rid of the line with the name of colums
       getline (PreviouslyAnnotatedFile,lineToParse);
       while ( getline (PreviouslyAnnotatedFile,lineToParse) ) {
           size_t pos = 0;
           std::string token;
           int EntryNumber=0;
           int PointNumber=0;
           int Points=1;
           int VideoNumber=0;
           int FrameNumber=0;

           while ((pos = lineToParse.find(delimiterRead)) != std::string::npos) {
               token = lineToParse.substr(0, pos);
               std::cout << token << std::endl;
               lineToParse.erase(0, pos + delimiterRead.length());


               if (EntryNumber==0){ //We are reading the VideoNumber
                   VideoNumber=stoi(token);
                   EntryNumber++;
               }
               else if (EntryNumber==1){ //We are reading the VideoNumber
                   FrameNumber=stoi(token);
                   EntryNumber++;
               }


               else if (EntryNumber==2 && Points==1){
                   VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointNumber].x=stoi(token);
                   EntryNumber++;
               }
               else if (EntryNumber==3 && Points==1){
                   VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointNumber].y=stoi(token);
                   EntryNumber=2;
                   PointNumber++;
                   if (PointNumber>=NpointsToTrack){
                       Points=0;
                       PointNumber=0;
                       VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].NumberOfTRegisteredPoints=NpointsToTrack;
                   }
               }
               else if (Points==0){
                   VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointNumber].Theta=stof(token);
                   PointNumber++;
               }
           }
           std::cout << lineToParse << std::endl;



       }
       myfile.close();
   }
   else{
       cout<<"\n No previous annotated data";
   }
}

int RatWalkTracker::ratWalkMain() {
   for (CurrentVideoAnalyzed=0;CurrentVideoAnalyzed<NumberOfVideos;CurrentVideoAnalyzed++){
        //VideoToAnalyze.OpenLiveStream(0); Not readu Yet
        
        
        //***********************************************************
        //Create a Window to display the video
        //***********************************************************
        namedWindow("RatWalkVideo",CV_WINDOW_AUTOSIZE);
        //imshow("RatWalkVideo", VideoToAnalyze.CurrentFrameData);
        int alpha_slider=0;
        int alpha_slider_max=VideoToAnalyze[CurrentVideoAnalyzed].NumberOfFrames;
        createTrackbar( "FrameNumberTrackBar", "RatWalkVideo", &alpha_slider, alpha_slider_max, ::on_trackbar, this );
        
        
        //Window for the zoomed region
        namedWindow("ZoomedRegion",CV_WINDOW_AUTOSIZE);
        
        //Show a Diagram with the points to Select
        //namedWindow("RatSkeletonControlPoints",CV_WINDOW_AUTOSIZE);
        //Read the Hip Conttol Point Skeleton Image
        Mat SkeletonImage;
        
        VideoToAnalyze[CurrentVideoAnalyzed].ShowSkeletonInCurrentFrame();
        
        while(1){
            
            //***********************************************************
            //Selec the frame to draw the points----------------------------------
            //***********************************************************
            cout<<"\n Please select a frame and add ponts pressing k";
            cout<<"CurrentFrame "<<VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame<<"\n";
            int Continue=SelectFrameForAddingTrackingPoints();
            

            
            
            if (Continue==0)
                break;
            
            //***********************************************************
            //Selec the points to track----------------------------------
            //***********************************************************
            bool SelectingTrakingPoints=true;
            char PressedKey;
            cout<<"\n Please select all the points to track";
            
            if ( VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].NumberOfTRegisteredPoints>0)
                VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].NumberOfTRegisteredPoints=0;
            
            
            
            //resizeWindow("RatSkeletonControlPoints",  100 ,  100);
            
            cvSetMouseCallback("RatWalkVideo", ::mouseHandlerForInitialTrackingPoints, this);
            while (SelectingTrakingPoints)
            {
                
                
                /*
                 
                 //Define the image to show depending in the point to be selected
                 if (PointID==0)
                 SkeletonImage = imread("RatSkeletonFrontHip.png", CV_LOAD_IMAGE_COLOR);   // Read the file
                 if (PointID==1)
                 SkeletonImage = imread("RatSkeletonFrontFemur.png", CV_LOAD_IMAGE_COLOR);   // Read the file
                 if (PointID==2)
                 SkeletonImage = imread("RatSkeletonFrontKnee.png", CV_LOAD_IMAGE_COLOR);   // Read the file
                 if (PointID==3)
                 SkeletonImage = imread("RatSkeletonFrontTalon.png", CV_LOAD_IMAGE_COLOR);   // Read the file
                 if (PointID==4)
                 SkeletonImage = imread("RatSkeletonFrontFeet.png", CV_LOAD_IMAGE_COLOR);   // Read the file
                 //n   imshow("RatSkeletonControlPoints", SkeletonImage); //Show it
                 */
                
                
                PressedKey= waitKey(1);
                
                if (PressedKey == 'N' || PressedKey == 'n') // TODO cristian pasar esto a eventos
                {
                    PointID++;
                    VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].NumberOfTRegisteredPoints++;
                    VideoToAnalyze[CurrentVideoAnalyzed].ShowSkeletonInCurrentFrame();
                    //If the five points have been selected, then we start the tracking process
                    if (PointID==NpointsToTrack){
                        //cvLine(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness=1, int line_type=8, int shift=0 )
                        SelectingTrakingPoints=false;
                        cout<<"Now select a new frame";
                        PointID=0;

                        cout<<"\n Points are Saved";


                        std::ofstream ofs (OutputFileName, std::ofstream::out);
                        ofs<<"VideoNumber,"<<"Frame,"<<"x1,"<<"y1,"<<"x2,"<<"y2,"<<"x3,"<<"y3,"<<"x4,"<<"y4,"<<"x5,"<<"y5,"<<"T1,"<<"T2,"<<"T3,"<<"T4,"<<"T5\n";


                        for (int VideoNumber=0;VideoNumber<NumberOfVideos;VideoNumber++){

                            for (int i=0;i<VideoToAnalyze[VideoNumber].NumberOfFrames;i++){
                                if (VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints>0){
                                    ofs<<VideoNumber<<",";
                                    ofs<<i<<",";
                                    for(int j=0; j< VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints;j++){
                                        ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].x<<",";
                                        ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].y<<", ";
                                    }
                                    for(int j=0; j< VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints;j++){
                                        if (j==VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints-1)
                                            ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].Theta<<"\n";
                                        else
                                            ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].Theta<<",";
                                    }

                                }
                            }

                        }
                        ofs.close();

                    }
                    else
                        cout<<"Select the next point";
                }

                
             //   if (PressedKey == 'S' || PressedKey == 's'){
                    //Save the Points

              //  }
                
                
                
                
                
            }
            
            
        }
        destroyAllWindows();
        
        VideoToAnalyze[CurrentVideoAnalyzed].RelaseVideo();
        return 0;
    }
   return 0;
}

void RatWalkTracker::nextFrame() {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetNextFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void RatWalkTracker::prevFrame() {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   VideoToAnalyze[CurrentVideoAnalyzed].GetPreviousFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void RatWalkTracker::guardar() {
   std::ofstream ofs (OutputFileName, std::ofstream::out);
   ofs<<"VideoNumber,"<<"Frame,"<<"x1,"<<"y1,"<<"x2,"<<"y2,"<<"x3,"<<"y3,"<<"x4,"<<"y4,"<<"x5,"<<"y5,"<<"T1,"<<"T2,"<<"T3,"<<"T4,"<<"T5\n";
   for (int VideoNumber=0;VideoNumber<NumberOfVideos;VideoNumber++) {
       for (int i=0;i<VideoToAnalyze[VideoNumber].NumberOfFrames;i++) {
           RatWalkFrameObject &frame = VideoToAnalyze[VideoNumber].FrameProperties[i];
           if (frame.NumberOfTRegisteredPoints > 0) {
               ofs<< VideoNumber << "," << i;
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs << "," << frame.TrackedPointsInFrame[j].x
                      << "," << frame.TrackedPointsInFrame[j].y;
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs << ",,";
               }
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs << ',' << frame.TrackedPointsInFrame[j].Theta;
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs << ',';
               }
               ofs << "\n";
           }
       }

   }
   ofs.close();
}

void RatWalkTracker::traeEsqueleto() {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   RatWalkFrameObject        &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame];
   bool sinAsignar = true;
   for (int i = currentVideo.CurrentFrame-1; sinAsignar && i >= 0; i--) {
      RatWalkFrameObject &prevFrame = currentVideo.FrameProperties[i];
      if (prevFrame.NumberOfTRegisteredPoints > 0) {
         currentFrame.NumberOfTRegisteredPoints = prevFrame.NumberOfTRegisteredPoints;
         for (int i = 0; i < prevFrame.NumberOfTRegisteredPoints; i++) {
            RatWalkControlPoint p = prevFrame.TrackedPointsInFrame[i];
            currentFrame.SetTrackedPoints(i, p.x, p.y);
         }
         PointID = std::min(4, currentFrame.NumberOfTRegisteredPoints);
         sinAsignar = false;
      }
   }
   if (sinAsignar) {
      currentFrame.NumberOfTRegisteredPoints = 5;
      PointID = 4;
      int step = currentVideo.Width / 6,
          x    = step,
          y    = currentVideo.Height / 2;
      for (int i = 0; i < 5; i++, x += step) {
         currentFrame.SetTrackedPoints(i, x, y);
      }
   }
}

Mat RatWalkTracker::getFrameWithRectangle() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithTrackingPoints();
}

Mat RatWalkTracker::getFrameWithSkeleton() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithSkeleton();
}

cv::Mat RatWalkTracker::getZoomedRegion(int x, int y, int frameWidth, int frameHeight) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   return currentVideo.getZoomedRegion(x2, y2, HalfWindowSize);
}

const RatWalkTrackerVideoObject &RatWalkTracker::getCurrentVideoAnalyzed() {
   return VideoToAnalyze[CurrentVideoAnalyzed];
}

const std::vector<string> &RatWalkTracker::getVideoNames() {
   return videoNames;
}

void RatWalkTracker::setCurrentVideo(int index) {
   CurrentVideoAnalyzed = index;
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void RatWalkTracker::addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, PointID);
   PointID = std::min(PointID+1, NpointsToTrack-1);
}

void RatWalkTracker::setPointOnCurrentFrame(int pointId, int x, int y,
                                            int frameWidth, int frameHeight) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, pointId);
}

int RatWalkTracker::getClosestPointID(int x, int y, int frameWidth,
                                      int frameHeight, double maxDistance) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   RatWalkFrameObject &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   double minDistance = -1.0, currentDistance;
   int minId = -1;
   for (int i = 0; i < currentFrame.NumberOfTRegisteredPoints; i++) {
      RatWalkControlPoint p = currentFrame.TrackedPointsInFrame[i];
      currentDistance = euclidianDistance(x2, y2, p.x, p.y);
      if (minDistance < 0 || currentDistance < minDistance) {
         minId = i;
         minDistance = currentDistance;
      }
   }
   if (minId != -1 && minDistance > maxDistance) {
      minId = -1;
   }
   return minId;
}

void RatWalkTracker::mouseHandlerForInitialTrackingPoints(int event, int x, int y, int flags) {
   if (event == CV_EVENT_LBUTTONDOWN)
   {

       VideoToAnalyze[CurrentVideoAnalyzed].SelectPoint(x, y, HalfWindowSize, PointID);
       // VideoToAnalyze.ShowSkeletonInCurrentFrame();

   }


   if (event == CV_EVENT_MOUSEMOVE)
   {
       Rect BoundingBoxData; /* bounding box */
       Mat FrameWithRectangle = VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrameData.clone();
       Mat FrameClone = VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrameData.clone();
       Mat ImageRegion;
       Mat ZoomedRegion;
       Point WindowPoint1, WindowPoint2;
       Point RegionPoint1, RegionPoint2;
       WindowPoint1.x=x-HalfWindowSize;
       if (WindowPoint1.x<0)
           WindowPoint1.x=0;
       WindowPoint1.y=y-HalfWindowSize;
       if (WindowPoint1.y<0)
           WindowPoint1.y=0;
       WindowPoint2.x=x+HalfWindowSize;
       if (WindowPoint2.x>VideoToAnalyze[CurrentVideoAnalyzed].Width-1)
           WindowPoint2.x=VideoToAnalyze[CurrentVideoAnalyzed].Width-1;
       WindowPoint2.y=y+HalfWindowSize;
       if (WindowPoint2.y>VideoToAnalyze[CurrentVideoAnalyzed].Height-1)
           WindowPoint2.y=VideoToAnalyze[CurrentVideoAnalyzed].Height-1;


       rectangle(FrameWithRectangle, WindowPoint1, WindowPoint2, CV_RGB(255, 0, 0), 3, 8, 0);
       BoundingBoxData = Rect(WindowPoint1.x,WindowPoint1.y,WindowPoint2.x-WindowPoint1.x,WindowPoint2.y-WindowPoint1.y);


       ImageRegion = FrameClone(BoundingBoxData);
       //rectangle(ImageRegion, RegionPoint1, RegionPoint2, CV_RGB(255, 0, 0), 1, 1, 0);
       circle(ImageRegion, Point(HalfWindowSize+1,HalfWindowSize+1),1, Scalar(255,0,0),CV_FILLED, 1,0);
       Size dsize = Size(500,500);
       resize(ImageRegion, ZoomedRegion, dsize);

       for (int PointToShow=0;PointToShow<VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].NumberOfTRegisteredPoints;PointToShow++)
           circle(FrameWithRectangle, Point((int)round(VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].TrackedPointsInFrame[PointToShow].x),(int)round(VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].TrackedPointsInFrame[PointToShow].y)),3, Scalar(0,0,255),CV_FILLED, 1,0);

       //VideoToAnalyze.ShowSkeletonInCurrentFrame();

       imshow("ZoomedRegion", ZoomedRegion);
       imshow("RatWalkVideo", FrameWithRectangle);
   }

   //    if (event == CV_EVENT_LBUTTONUP && drag)
   //    {
   //        point2 = Point(x, y);
   //        rect = Rect(point1.x,point1.y,x-point1.x,y-point1.y);
   //        drag = 0;
   //        if (select_flag==0)
   //            roiImg1 = img(rect);
   //        if (select_flag==1)
   //            roiImg2 = img(rect);
   //        if (select_flag==2)
   //            roiImg3 = img(rect);
   //    }

   //    if (event == CV_EVENT_LBUTTONUP)
   //    {
   /* ROI selected */
   //        select_flag ++;
   //        drag = 0;
   //    }
}

/*####################################################
 Routine for Selecting frame for drwawing the tracking points
 #####################################################*/
int RatWalkTracker::SelectFrameForAddingTrackingPoints() {


   //Forward
   char PressedKey=waitKey(0);  //Check if we can change this for a one
   while(PressedKey!='K' && PressedKey!='k'){


       /*
        //Show the points in the frame
        Mat FrameWithRectangle = VideoToAnalyze.CurrentFrameData.clone();
        for (int PointToShow=0;PointToShow<VideoToAnalyze.FrameProperties[VideoToAnalyze.CurrentFrame].NumberOfTrackedPoints;PointToShow++){
        circle(FrameWithRectangle, Point((int)round(VideoToAnalyze.FrameProperties[VideoToAnalyze.CurrentFrame].TrackedPointsInFrame[PointToShow].CoorX),(int)round(VideoToAnalyze.FrameProperties[VideoToAnalyze.CurrentFrame].TrackedPointsInFrame[PointToShow].CoorY)),3, Scalar(0,0,255),CV_FILLED, 1,0);

        }
        imshow("RatWalkVideo", FrameWithRectangle);
        */




       if (PressedKey=='N' || PressedKey=='n'){
           VideoToAnalyze[CurrentVideoAnalyzed].GetNextFrame();
           cout<<"CurrentFrame "<<VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame<<"\n";
       }

       //Backwards
       if (PressedKey=='P' || PressedKey=='p'){
           VideoToAnalyze[CurrentVideoAnalyzed].GetPreviousFrame();
           cout<<"CurrentFrame "<<VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame<<"\n";
       }
       VideoToAnalyze[CurrentVideoAnalyzed].ShowSkeletonInCurrentFrame();

       if (PressedKey==27)
           return 0;
       //imshow("RatWalkVideo", VideoToAnalyze.CurrentFrameData);
       PressedKey=waitKey(0);
   }
   return 1;
}

void RatWalkTracker::on_trackbar(int Position) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetFrameNumber((double)Position);
   currentVideo.ShowSkeletonInCurrentFrame();
   cout<<"CurrentFrame "<<VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame<<"\n";
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}
