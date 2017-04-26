#include "RatWalkCore/Project.h"

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

#include <QString>

#include "RatWalkCore/Video.h"
#include "RatWalkCore/Constantes.h"
#include "RatWalkCore/Points.h"

using namespace  cv;
using namespace std;

constexpr int HALF_WINDOW_SIZE = 9;

namespace RatWalkCore {

Project::Project(const char *fileName) :
   ratFile(fileName),
   corrector(VideoToAnalyze) {
   CurrentVideoAnalyzed=0;

   //Open the video files
   for (int i = 0; i < ratFile.numberOfVideos(); i++) {
      VideoToAnalyze.push_back(Video());
      stepRegisters.push_back(StepRegister());
      const char *FileNameToRead = ratFile.getVideoFilename(i);
      if (!VideoToAnalyze[i].OpenVideoFile((char *)FileNameToRead)) {
         cout<<"Video File "<<FileNameToRead <<" Could not be opened";
         return;
//         return 0;
      }
   }

   loadStepRegister(ratFile.getStepRegisterFilename());
   corrector.performCorrection(ratFile.getTargetFilename());

   //Try to read the previously annotated things
   string lineToParse;

   ifstream PreviouslyAnnotatedFile(ratFile.getOutputFilenameWidthPath());
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

           for (int i = 0; i < NUMBER_OF_POINTS_TO_TRACK && !tokens[i*2+2].empty(); i++) {
               frame.TrackedPointsInFrame[i].CoorX  = stoi(tokens[i*2+2]);
               frame.TrackedPointsInFrame[i].CoorY  = stoi(tokens[i*2+3]);
               frame.TrackedPointsInFrame[i].Theta = stod(tokens[i+12]);
               frame.NumberOfTRegisteredPoints++;
           }
           PointID = std::min(NUMBER_OF_POINTS_TO_TRACK-1, frame.NumberOfTRegisteredPoints);
       }
       //Create the corrected

       for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++){
           for (int FrameNumber=0;FrameNumber<VideoToAnalyze[VideoNumber].NumberOfFrames;FrameNumber++){
               for (int PointId=0;PointId<NUMBER_OF_POINTS_TO_TRACK;PointId++){
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

void Project::nextFrame() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetNextFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Project::prevFrame() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   VideoToAnalyze[CurrentVideoAnalyzed].GetPreviousFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Project::save() {
   const char *HEADER = "VideoNumber,Frame,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,T1,T2,T3,T4,T5\n";
   std::ofstream ofs(ratFile.getOutputFilenameWidthPath(), std::ofstream::out);
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
   saveStepRegister(ratFile.getStepRegisterFilename());
}

void Project::bringPreviousSkeleton() {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed]; //!< \todo Project debería tener un método currentVideo()
   Frame &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame]; //!< \todo Video debería tener un método currentFrame()
   bool sinAsignar = true;
   for (int i = currentVideo.CurrentFrame-1; sinAsignar && i >= 0; i--) {
      Frame &prevFrame = currentVideo.FrameProperties[i];
      if (prevFrame.NumberOfTRegisteredPoints > 0) {
         currentFrame.NumberOfTRegisteredPoints = 0;
         for (int i = 0; i < prevFrame.NumberOfTRegisteredPoints; i++) {
            ControlPoint p = prevFrame.TrackedPointsInFrame[i];

            currentVideo.SelectPoint(p.CoorX, p.CoorY, HALF_WINDOW_SIZE, i, CurrentVideoAnalyzed);
            //currentFrame.SetTrackedPoints(i, p.x, p.y);
         }
         PointID = std::min(NUMBER_OF_POINTS_TO_TRACK-1, currentFrame.NumberOfTRegisteredPoints);
         sinAsignar = false;
      }
   }
   if (sinAsignar) {
      currentFrame.NumberOfTRegisteredPoints = 0;
      PointID = NUMBER_OF_POINTS_TO_TRACK-1;
      int step = currentVideo.Width / 6,
          x    = step,
          y    = currentVideo.Height / 2;
      for (int i = 0; i < 5; i++, x += step) {
         currentVideo.SelectPoint(x, y, HALF_WINDOW_SIZE, i, CurrentVideoAnalyzed);
      }
   }
}

Mat Project::getFrameWithRectangle() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithTrackingPoints();
}

Mat Project::getFrameWithSkeleton() {
   return VideoToAnalyze[CurrentVideoAnalyzed].getFrameWithSkeleton();
}

cv::Mat Project::getZoomedRegion(int x, int y, int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   return currentVideo.getZoomedRegion(x2, y2, HALF_WINDOW_SIZE);
}

const Video &Project::getCurrentVideoAnalyzed() {
   return VideoToAnalyze[CurrentVideoAnalyzed];
}

const std::vector<string> &Project::getVideoFilenames() {
   return ratFile.getVideoFilenames();
}

void Project::setCurrentVideo(int index) {
   CurrentVideoAnalyzed = index;
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

void Project::saveCorrectedFile() {
    std::ofstream ofsCorrected(ratFile.getOutputFilenameCorrected());
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

std::vector<Video> &Project::getVideos() {
    return VideoToAnalyze;
}

std::vector<StepRegister> &Project::getStepRegisters() {
    return stepRegisters;
}

StepRegister &Project::getCurrentStepRegister() {
    return stepRegisters[CurrentVideoAnalyzed];
}

int Project::getCurrentVideoIndex() {
    return CurrentVideoAnalyzed;
}

void Project::loadStepRegister(const char *filename) {
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

void Project::saveStepRegister(const char *filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return;
    for (int i = 0; i < 3; i++) {
        for (auto step : stepRegisters[i].getSteps()) {
            outFile << i << ',' << step.first << ',' << step.second << std::endl;
        }
    }
}

const char *Project::getProjectName() {
   return ratFile.getProjectName();
}

int Project::getSize() {
   return VideoToAnalyze.size();
}

void Project::addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HALF_WINDOW_SIZE, PointID, CurrentVideoAnalyzed);
   PointID = std::min(PointID+1, NUMBER_OF_POINTS_TO_TRACK-1);
}

void Project::setPointOnCurrentFrame(int pointId, int x, int y,
                                            int frameWidth, int frameHeight) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HALF_WINDOW_SIZE, pointId, CurrentVideoAnalyzed);
}

void Project::deletePointOnCurrentFrame(int pointId) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   Frame &currentFrame = currentVideo.FrameProperties[currentVideo.CurrentFrame];
   auto iter = currentFrame.TrackedPointsInFrame.begin() + pointId;
   currentFrame.TrackedPointsInFrame.erase(iter);
   currentFrame.TrackedPointsInFrame.push_back(ControlPoint());
   currentFrame.NumberOfTRegisteredPoints--;
   currentFrame.NumberOfTRegisteredPoints = PointID;
}

int Project::getClosestPointID(int x, int y, int frameWidth,
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

void Project::setFrame(int Position) {
   Video &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   currentVideo.GetFrameNumber((double)Position);
   currentVideo.ShowSkeletonInCurrentFrame();
   PointID = currentVideo.FrameProperties[currentVideo.CurrentFrame].NumberOfTRegisteredPoints;
   PointID = std::min(PointID, 4);
}

} // namespace RatWalkCore
