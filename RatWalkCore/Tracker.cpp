#include "RatWalkCore/Tracker.h"

#include <unistd.h>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "RatWalkCore/Video.h"
#include "RatWalkCore/Constantes.h"
#include "RatWalkCore/Points.h"
#include <QDebug>

cv::Mat HLeft, HMiddle, HRight; // extern global variables

using namespace  cv;
using namespace std;

namespace RatWalkCore {

Tracker::Tracker(const char *fileName) :
   ratFile(fileName) {
   CurrentVideoAnalyzed=0;

   //Open the video files
   for (int i = 0; i < ratFile.numberOfVideos(); i++) {
      std::string fileString     = ratFile.getVideoFilenameWithPath(i); // soulución al error en ubuntu
      const char *FileNameToRead = fileString.c_str();
      if (!VideoToAnalyze[i].OpenVideoFile((char *)FileNameToRead)) {
         cout<<"Video File "<<FileNameToRead <<" Could not be opened";
         return;
//         return 0;
      }
   }

   loadStepRegister(ratFile.getStepRegisterFilename().c_str());

   ////////////
   //PERFORM THE CORRECTION OF THE VIDEOS
   /////////////

       std::string targetFilename = ratFile.getTargetFilename();
       Mat OriginalTargetImage= imread(targetFilename);
       //Mat OriginalTargetImage= imread( "CalibrationShoes.png");
       double Scale=480/(double)OriginalTargetImage.rows;
       resize(OriginalTargetImage, TargetImage, Size(0,0),Scale,Scale,INTER_LINEAR);
       cvtColor(TargetImage, TargetImageGrayG, CV_RGB2GRAY);

       Image1=VideoToAnalyze[0].CurrentFrameData.clone();
       Image2=VideoToAnalyze[1].CurrentFrameData.clone();
       Image3=VideoToAnalyze[2].CurrentFrameData.clone();


       //Convert the images to grayScale
       cout<<"Convert the images to grayScale";

       cvtColor(Image1, ImageLeftG, CV_RGB2GRAY);
       cvtColor(Image2, ImageMiddleG, CV_RGB2GRAY);
       cvtColor(Image3, ImageRightG, CV_RGB2GRAY);

       int blursize=1;
       blur(TargetImageGrayG, TargetImageGray, Size(blursize,blursize));
       blur(ImageLeftG, ImageLeft, Size(blursize,blursize));
       blur(ImageMiddleG, ImageMiddle, Size(blursize,blursize));
       blur(ImageRightG, ImageRight, Size(blursize,blursize));

       //Detect KeyPoints using SURF detector
       cout<<"Detect KeyPoints using SURF detector";
       //int MinHessian = 400;
       //SurfrereDetector detector(MinHessian);
       SiftFeatureDetector detector;
       std::vector<KeyPoint> keyPointsImageLeft, KeyPointsImageMiddle, KeyPointsImageRight, KeyPointsTargetImage;

       detector.detect(ImageLeft, keyPointsImageLeft);
       detector.detect(ImageMiddle, KeyPointsImageMiddle);
       detector.detect(ImageRight, KeyPointsImageRight);
       detector.detect(TargetImageGray, KeyPointsTargetImage);

       //Calculate descriptors (feature vectors)
       cout<<"Calculate descriptors (feature vectors)";
       //SurfDescriptorExtractor extractor;
       SiftDescriptorExtractor extractor;
       Mat DescriptorsImageLeft, DescriptorsImageRight, DescriptorsImageMidde,DescriptorsTargetImage;

       extractor.compute(ImageLeft, keyPointsImageLeft, DescriptorsImageLeft);
       extractor.compute(ImageMiddle, KeyPointsImageMiddle, DescriptorsImageMidde);
       extractor.compute(ImageRight, KeyPointsImageRight, DescriptorsImageRight);
       extractor.compute(TargetImageGray, KeyPointsTargetImage, DescriptorsTargetImage);

       //Maching descriptor vectors using FLANN matcher
       cout<<"Maching descriptor vectors using FLANN matcher";
       FlannBasedMatcher matcher;
       vector<DMatch> MatchesImageLeft,MatchesImageMiddle,MatchesImageRight;

       matcher.match(DescriptorsImageLeft, DescriptorsTargetImage, MatchesImageLeft);
       matcher.match(DescriptorsImageMidde, DescriptorsTargetImage, MatchesImageMiddle);
       matcher.match(DescriptorsImageRight, DescriptorsTargetImage, MatchesImageRight);

       //Quick calculation of max and min distances between keypoints
       cout<<"Quick calculation of max and min distances between keypoints";
       double max_distLeft = 0; double min_distLeft = 100;
       for (int i=0;i<DescriptorsImageLeft.rows;i++){
           double distLeft = MatchesImageLeft[i].distance;
           if( distLeft < min_distLeft ) min_distLeft = distLeft;
           if( distLeft > max_distLeft ) max_distLeft = distLeft;
       }
       double max_distMiddle= 0; double min_distMiddle= 100;
       for (int i=0;i<DescriptorsImageMidde.rows;i++){
           double distMiddle = MatchesImageMiddle[i].distance;
           if( distMiddle < min_distMiddle) min_distMiddle = distMiddle;
           if( distMiddle > max_distMiddle ) max_distMiddle = distMiddle;
       }
       double max_distRight = 0; double min_distRight = 100;
       for (int i=0;i<DescriptorsImageRight.rows;i++){
           double distRight = MatchesImageRight[i].distance;
           if( distRight< min_distRight) min_distRight = distRight;
           if( distRight> max_distRight ) max_distRight = distRight;
       }



       //Keep only the good matches
       cout<<"Keep only the good matches";
       //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
       std::vector< DMatch > good_matchesLeft,good_matchesMiddle,good_matchesRight;

       for( int i = 0; i < DescriptorsImageLeft.rows; i++ )
       { if( MatchesImageLeft[i].distance < 3*min_distLeft )
       { good_matchesLeft.push_back( MatchesImageLeft[i]); }
       }
       for( int i = 0; i < DescriptorsImageMidde.rows; i++ )
       { if( MatchesImageMiddle[i].distance < 3*min_distMiddle )
       { good_matchesMiddle.push_back( MatchesImageMiddle[i]); }
       }
       for( int i = 0; i < DescriptorsImageRight.rows; i++ )
       { if( MatchesImageRight[i].distance < 3*min_distRight )
       { good_matchesRight.push_back( MatchesImageRight[i]); }
       }


       Mat img_matches;
       drawMatches( ImageMiddle , KeyPointsImageMiddle, TargetImageGray, KeyPointsTargetImage,
                   good_matchesMiddle, img_matches, Scalar::all(-1), Scalar::all(-1),
                   vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );


       //  imshow("MatchesLeft",img_matches);
       //  waitKey(1);



       //-- Compute the Trasnformation Matrix
       std::vector<Point2f> ImageLeftControlPoints,ImageRightControlPoints,ImageMiddleControlPoints;
       std::vector<Point2f> TargetImageControlPointsLeft,TargetImageControlPointsMiddle,TargetImageControlPointsRight;
       for( int i = 0; i < good_matchesLeft.size(); i++ )
       {
           //-- Get the keypoints from the good matches
           ImageLeftControlPoints.push_back( keyPointsImageLeft[ good_matchesLeft[i].queryIdx ].pt );
           TargetImageControlPointsLeft.push_back( KeyPointsTargetImage[ good_matchesLeft[i].trainIdx ].pt );
       }
       HLeft = findHomography( ImageLeftControlPoints, TargetImageControlPointsLeft, CV_RANSAC );

       for( int i = 0; i < good_matchesMiddle.size(); i++ )
       {
           //-- Get the keypoints from the good matches
           ImageMiddleControlPoints.push_back( KeyPointsImageMiddle[ good_matchesMiddle[i].queryIdx ].pt );
           TargetImageControlPointsMiddle.push_back( KeyPointsTargetImage[ good_matchesMiddle[i].trainIdx ].pt );
       }
       HMiddle = findHomography( ImageMiddleControlPoints, TargetImageControlPointsMiddle, CV_RANSAC );

       for( int i = 0; i < good_matchesRight.size(); i++ )
       {
           //-- Get the keypoints from the good matches
           ImageRightControlPoints.push_back( KeyPointsImageRight[ good_matchesRight[i].queryIdx ].pt );
           TargetImageControlPointsRight.push_back( KeyPointsTargetImage[ good_matchesRight[i].trainIdx ].pt );
       }
       HRight = findHomography( ImageRightControlPoints, TargetImageControlPointsRight, CV_RANSAC );




       //SaveCalibrationMatrices
       cv::FileStorage storage("CalibrationParameters.xml", cv::FileStorage::WRITE);
       storage << "HLeft" << HLeft;
       storage << "HMiddle" << HMiddle;
       storage << "HRight" << HRight;
       storage.release();





       // Use the Homography Matrix to warp the images
       cv::Mat resultLeft;
       warpPerspective(Image1,resultLeft,HLeft,cv::Size(TargetImage.cols,TargetImage.rows));
       cv::Mat resultMiddle;
       warpPerspective(Image2,resultMiddle,HMiddle,cv::Size(TargetImage.cols,TargetImage.rows));
       cv::Mat resultRight;
       warpPerspective(Image3,resultRight,HRight,cv::Size(TargetImage.cols,TargetImage.rows));


       /*
       cv::imshow("left", resultLeft);
       cv::imshow("middle", resultMiddle);
       cv::imshow("right", resultRight);
       // waitKey(0);

       */


       Mat FinalResutl(TargetImage.rows,TargetImage.cols, CV_8UC3, Scalar(0, 0, 0));


       for(int i = 0; i < FinalResutl.rows; i++)
       {
           for(int j = 0; j < FinalResutl.cols; j++)
           {
               uchar PixelValueLeftR=resultLeft.at<cv::Vec3b>(i,j)[0];
               uchar PixelValueLeftG=resultLeft.at<cv::Vec3b>(i,j)[1];
               uchar PixelValueLeftB=resultLeft.at<cv::Vec3b>(i,j)[2];

               uchar PixelValueMiddleR=resultMiddle.at<cv::Vec3b>(i,j)[0];
               uchar PixelValueMiddleG=resultMiddle.at<cv::Vec3b>(i,j)[1];
               uchar PixelValueMiddleB=resultMiddle.at<cv::Vec3b>(i,j)[2];

               uchar PixelValueRightR=resultRight.at<cv::Vec3b>(i,j)[0];
               uchar PixelValueRightG=resultRight.at<cv::Vec3b>(i,j)[1];
               uchar PixelValueRightB=resultRight.at<cv::Vec3b>(i,j)[2];

               float SumaR=0,SumaG=0,SumaB=0,NPix=0;

               if (PixelValueLeftR+PixelValueLeftG+PixelValueLeftB!=0){
                   SumaR+=(float)PixelValueLeftR;
                   SumaG+=(float)PixelValueLeftG;
                   SumaB+=(float)PixelValueLeftB;
                   NPix++;
               }
               if (PixelValueRightR+ PixelValueRightG+PixelValueRightB!=0){
                   SumaR+=(float)PixelValueRightR;
                   SumaG+=(float)PixelValueRightG;
                   SumaB+=(float)PixelValueRightB;
                   NPix++;
               }
               if (PixelValueMiddleR+PixelValueMiddleG+PixelValueMiddleB!=0){
                   SumaR+=(float)PixelValueMiddleR;
                   SumaG+=(float)PixelValueMiddleG;
                   SumaB+=(float)PixelValueMiddleB;
                   NPix++;
               }
               if (NPix>0){
                   FinalResutl.at<cv::Vec3b>(i,j)[0] =(uchar)round(SumaR/NPix);
                   FinalResutl.at<cv::Vec3b>(i,j)[1] =(uchar)round(SumaG/NPix);
                   FinalResutl.at<cv::Vec3b>(i,j)[2] =(uchar)round(SumaB/NPix);
               }

           }
       }



       //imshow( "FinalResutl", FinalResutl );



   ////////////
   // FIN DEL PERFORM THE CORRECTION OF THE VIDEOS
   /////////////

   //Try to read the previously annotated things
   string lineToParse;

   ifstream PreviouslyAnnotatedFile(ratFile.getOutputFilenameWidthPath().c_str());
   string delimiterRead = ",";
   if (PreviouslyAnnotatedFile.is_open()) {
       //Get rid of the line with the name of colums
       getline(PreviouslyAnnotatedFile, lineToParse);
       while ( getline(PreviouslyAnnotatedFile, lineToParse) ) {
           size_t pos = 0;
           std::string token;
           int  VideoNumber = 0;
           int  FrameNumber = 0;
           std::vector<std::string> tokens;

           do {
              pos = lineToParse.find(delimiterRead);
              token = lineToParse.substr(0, pos);
              tokens.push_back(token);
              lineToParse.erase(0, pos + delimiterRead.length());
           } while (pos != std::string::npos);

           VideoNumber = stoi(tokens[0]);
           FrameNumber = stoi(tokens[1]);

           Frame &frame = VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber];

           for (int i = 0; i < NpointsToTrack && !tokens[i*2+2].empty(); i++) {
               frame.TrackedPointsInFrame[i].CoorX  = stoi(tokens[i*2+2]);
               frame.TrackedPointsInFrame[i].CoorY  = stoi(tokens[i*2+3]);
               frame.TrackedPointsInFrame[i].Theta = stod(tokens[i+12]);
               frame.NumberOfTRegisteredPoints++;
           }
           PointID = std::min(NpointsToTrack-1, frame.NumberOfTRegisteredPoints);
       }
       //Create the corrected

       for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++){
           for (int FrameNumber=0;FrameNumber<VideoToAnalyze[VideoNumber].NumberOfFrames;FrameNumber++){
               for (int PointId=0;PointId<NUMBEROFPOINTSTOTRACK;PointId++){
                   int x=VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorX;
                   int y=VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorY;
                   std::vector<Point2f> vec;
                   std::vector<Point2f> vecCorrected;
                   vec.push_back(Point2f(x,y));
                   if (VideoNumber==0)
                       cv::perspectiveTransform(vec,vecCorrected, HLeft);
                   if (VideoNumber==1)
                       cv::perspectiveTransform(vec,vecCorrected, HMiddle);
                   if (VideoNumber==2)
                       cv::perspectiveTransform(vec,vecCorrected, HRight);

                   VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorXCorrected=vecCorrected[0].x;
                   VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorYCorrected=vecCorrected[0].y;

                   if (PointId>0){
                       double xa=(double)  VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId-1].CoorXCorrected;
                       double ya=(double)  VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId-1].CoorYCorrected;
                       double xb=(double)  VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorXCorrected;
                       double yb=(double)  VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId].CoorYCorrected;

                       double Angle=180*atan2((yb-ya),(xb-xa))/3.1416;
                        VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[PointId-1].ThetaCorrected=Angle;
                   }


                   if (PointId==4){
                       double xa=(double) VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[0].CoorXCorrected;
                       double ya=(double) VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[0].CoorYCorrected;
                       double xb=(double) VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[3].CoorXCorrected;
                       double yb=(double) VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[3].CoorYCorrected;

                       double Angle=180*atan2((yb-ya),(xb-xa))/3.1416;
                       VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber].TrackedPointsInFrame[4].ThetaCorrected=Angle;
                   }




               }
           }
       }
       //Save the previously Annotated Corrected
        saveCorrectedFile();
   } else {
       cout<<"\n No previous annotated data";
   }
}

void Tracker::nextFrame() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetNextFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Tracker::prevFrame() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   VideoToAnalyze[CurrentVideoAnalyzed].GetPreviousFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Tracker::guardar() {
   const char *HEADER = "VideoNumber,Frame,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,T1,T2,T3,T4,T5\n";
   std::ofstream ofs(ratFile.getOutputFilenameWidthPath().c_str(), std::ofstream::out);
   ofs  << HEADER;
   for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++) {
       for (int i = 0 ;i < VideoToAnalyze[VideoNumber].NumberOfFrames; i++) {
           Frame &frame = VideoToAnalyze[VideoNumber].FrameProperties[i];
           if (frame.NumberOfTRegisteredPoints > 0) {
               ofs << VideoNumber << "," << i;
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs << "," << frame.TrackedPointsInFrame[j].CoorX
                      << "," << frame.TrackedPointsInFrame[j].CoorY;
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs << ",,";
               }
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs << ',' << QString::number(frame.TrackedPointsInFrame[j].Theta, 'f', 10).toStdString();
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs << ',';
               }
               ofs << "\n";
           }
       }

   }
   ofs.close();
   saveCorrectedFile();
   saveStepRegister(ratFile.getStepRegisterFilename().c_str());
}

void Tracker::traeEsqueleto() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed]; //!< \todo Tracker debería tener un método currentVideo()
   Frame &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame]; //!< \todo Video debería tener un método currentFrame()
   bool sinAsignar = true;
   for (int i = currentVideo.CurrentFrame-1; sinAsignar && i >= 0; i--) {
      Frame &prevFrame = currentVideo.FrameProperties[i];
      if (prevFrame.NumberOfTRegisteredPoints > 0) {
         currentFrame.NumberOfTRegisteredPoints = 0;
         for (int i = 0; i < prevFrame.NumberOfTRegisteredPoints; i++) {
            ControlPoint p = prevFrame.TrackedPointsInFrame[i];

            currentVideo.SelectPoint(p.CoorX, p.CoorY, HalfWindowSize, i, CurrentVideoAnalyzed);
            //currentFrame.SetTrackedPoints(i, p.x, p.y);
         }
         PointID = std::min(NpointsToTrack-1, currentFrame.NumberOfTRegisteredPoints);
         sinAsignar = false;
      }
   }
   if (sinAsignar) {
      currentFrame.NumberOfTRegisteredPoints = 0;
      PointID = NpointsToTrack-1;
      int step = currentVideo.Width / 6,
          x    = step,
          y    = currentVideo.Height / 2;
      for (int i = 0; i < 5; i++, x += step) {
         currentVideo.SelectPoint(x, y, HalfWindowSize, i, CurrentVideoAnalyzed);
      }
   }
}

Mat Tracker::getFrameWithRectangle() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithTrackingPoints();
}

Mat Tracker::getFrameWithSkeleton() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithSkeleton();
}

cv::Mat Tracker::getZoomedRegion(int x, int y, int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   return currentVideo.getZoomedRegion(x2, y2, HalfWindowSize);
}

const Video &Tracker::getCurrentVideoAnalyzed() {
   return VideoToAnalyze[CurrentVideoAnalyzed];
}

const std::vector<string> &Tracker::getVideoNames() {
   return ratFile.getVideoNames();
}

void Tracker::setCurrentVideo(int index) {
   CurrentVideoAnalyzed = index;
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Tracker::saveCorrectedFile() {
    std::ofstream ofsCorrected(ratFile.getOutputFilenameCorrected().c_str());
    ofsCorrected << "VideoNumber,Frame,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,T1,T2,T3,T4,T5\n";
    for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++) {
        Video &video = VideoToAnalyze[VideoNumber];
        for (int frameNumber = 0; frameNumber < video.NumberOfFrames; frameNumber++) {
            Frame &frame = video.FrameProperties[frameNumber];
            ofsCorrected << VideoNumber << "," << frameNumber;
            for(int pointNumber = 0; pointNumber < frame.NumberOfPointsToTrack; pointNumber++) {
                if (pointNumber < frame.NumberOfTRegisteredPoints) {
                    ControlPoint &point = frame.TrackedPointsInFrame[pointNumber];
                    ofsCorrected << "," << point.CoorXCorrected
                                 << "," << point.CoorYCorrected;
                } else {
                    ofsCorrected << ",-1,-1";
                }
            }
            for(int pointNumber = 0; pointNumber < frame.NumberOfPointsToTrack; pointNumber++) {
                if (pointNumber < frame.NumberOfTRegisteredPoints) {
                    ControlPoint &point = frame.TrackedPointsInFrame[pointNumber];
                    ofsCorrected << ',' << QString::number(point.ThetaCorrected, 'f', 10).toStdString();
                } else {
                    ofsCorrected << ",-1";
                }
            }
            ofsCorrected << '\n';
        }
    }
    ofsCorrected.close();
}

Video *Tracker::getVideos() {
    return VideoToAnalyze;
}

StepRegister *Tracker::getStepRegisters() {
    return stepRegisters;
}

StepRegister &Tracker::getCurrentStepRegister() {
    return stepRegisters[CurrentVideoAnalyzed];
}

int Tracker::getCurrentVideoIndex() {
    return CurrentVideoAnalyzed;
}

void Tracker::loadStepRegister(const char *filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        int video, stepBegin, stepEnd;
        while (!(inFile >> video).eof()) {
            while (inFile.get() != ',') continue;
            inFile >> stepBegin;
            while (inFile.get() != ',') continue;
            inFile >> stepEnd;
            stepRegisters[video].addStep(stepBegin, stepEnd);
        }
    }
}

void Tracker::saveStepRegister(const char *filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return;
    for (int i = 0; i < 3; i++) {
        for (auto step : stepRegisters[i].getSteps()) {
            outFile << i << ',' << step.first << ',' << step.second << std::endl;
        }
    }
}

void Tracker::addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, PointID, CurrentVideoAnalyzed);
   PointID = std::min(PointID+1, NpointsToTrack-1);
}

void Tracker::setPointOnCurrentFrame(int pointId, int x, int y,
                                            int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, pointId, CurrentVideoAnalyzed);
}

void Tracker::deletePointOnCurrentFrame(int pointId) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   Frame &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame];
   auto iter = currentFrame.TrackedPointsInFrame.begin() + pointId;
   currentFrame.TrackedPointsInFrame.erase(iter);
   currentFrame.TrackedPointsInFrame.push_back(ControlPoint());
   currentFrame.NumberOfTRegisteredPoints--;
   currentFrame.NumberOfTRegisteredPoints = PointID;
}

int Tracker::getClosestPointID(int x, int y, int frameWidth,
                                      int frameHeight, double maxDistance) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   Frame &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   double minDistance = -1.0, currentDistance;
   int minId = -1;
   for (int i = 0; i < currentFrame.NumberOfTRegisteredPoints; i++) {
      ControlPoint p = currentFrame.TrackedPointsInFrame[i];
      currentDistance = euclidianDistance(x2, y2, p.CoorX, p.CoorY);
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

void Tracker::setFrame(int Position) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetFrameNumber((double)Position);
   currentVideo.ShowSkeletonInCurrentFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

} // namespace RatWalkCore
