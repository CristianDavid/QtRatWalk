#include "RatWalkTracker.h"

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

#include "RatWalkTrackerVideoObject.h"
#include "RatWalkConstantes.h"
#include "Points.h"
#include <QDebug>

cv::Mat HLeft, HMiddle, HRight; // extern global variables

using namespace  cv;
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

RatWalkTracker::RatWalkTracker(const char *fileName) :
   ratFile(fileName) {
   CurrentVideoAnalyzed=0;

   //Open the video files
   for (int i = 0; i < ratFile.numberOfVideos(); i++) {
      const char *FileNameToRead = ratFile.getVideoFilenameWithPath(i).c_str();
      if (!VideoToAnalyze[i].OpenVideoFile((char *)FileNameToRead)) {
         cout<<"Video File "<<FileNameToRead <<" Could not be opened";
         return;
//         return 0;
      }
   }

   ////////////
   //PERFORM THE CORRECTION OF THE VIDEOS
   /////////////

       std::string targetFilename = ratFile.getTargetFilename();
       Mat OriginalTargetImage= imread(targetFilename);
       //Mat OriginalTargetImage= imread( "CalibrationShoes.png");
       double Scale=480/(double)OriginalTargetImage.rows;
       resize(OriginalTargetImage, TargetImage, Size(0,0),Scale,Scale,INTER_LINEAR);
       cvtColor(TargetImage, TargetImageGrayG, CV_RGB2GRAY);


   //    //Open the videos=====================================
   //    VideoCapture VideoLeft, VideoMiddle, VideoRight;
   //    VideoLeft.open(argv[1]);
   //    VideoMiddle.open(argv[2]);
   //    VideoRight.open(argv[3]);
   //
   //
   //    VideoLeft.read(Image1);
   //    VideoMiddle.read(Image2);
   //    VideoRight.read(Image3);


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

           /*while ((pos = lineToParse.find(delimiterRead)) != std::string::npos) {
              token = lineToParse.substr(0, pos);
              tokens.push_back(token);
              lineToParse.erase(0, pos + delimiterRead.length());
           }*/
           do {
              pos = lineToParse.find(delimiterRead);
              token = lineToParse.substr(0, pos);
              tokens.push_back(token);
              lineToParse.erase(0, pos + delimiterRead.length());
           } while (pos != std::string::npos);

           VideoNumber = stoi(tokens[0]);
           FrameNumber = stoi(tokens[1]);

           RatWalkFrameObject &frame = VideoToAnalyze[VideoNumber].FrameProperties[FrameNumber];

           for (int i = 0; i < NpointsToTrack && !tokens[i*2+2].empty(); i++) {
               frame.TrackedPointsInFrame[i].CoorX  = stoi(tokens[i*2+2]);
               frame.TrackedPointsInFrame[i].CoorY  = stoi(tokens[i*2+3]);
               frame.TrackedPointsInFrame[i].Theta = stod(tokens[i+12]);
               frame.NumberOfTRegisteredPoints++;
           }
           PointID = std::min(NpointsToTrack-1, frame.NumberOfTRegisteredPoints);
       }
       //Create the corrected


       // TODO adaptar esto a la forma en la que guardo
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
       // cristian 17112016
       std::ofstream ofsCorrected(ratFile.getOutputFilenameCorrected().c_str());
       ofsCorrected << "VideoNumber,Frame,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,T1,T2,T3,T4,T5\n";
       for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++) {
           for (int i = 0 ;i < VideoToAnalyze[VideoNumber].NumberOfFrames; i++) {
               RatWalkFrameObject &frame = VideoToAnalyze[VideoNumber].FrameProperties[i];
               if (frame.NumberOfTRegisteredPoints > 0) {
                   ofsCorrected << VideoNumber << "," << i;
                   for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                      ofsCorrected << "," << frame.TrackedPointsInFrame[j].CoorXCorrected
                          << "," << frame.TrackedPointsInFrame[j].CoorYCorrected;
                   }
                   for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                      ofsCorrected << ",,";
                   }
                   for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                      ofsCorrected << ',' << QString::number(frame.TrackedPointsInFrame[j].ThetaCorrected, 'f', 10).toStdString();
                   }
                   for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                      ofsCorrected << ',';
                   }
                   ofsCorrected << '\n';
               }
           }
       }
       ofsCorrected.close();
       // fin cristian
   } else {
       cout<<"\n No previous annotated data";
   }
}

int RatWalkTracker::ratWalkMain() {
   for (CurrentVideoAnalyzed=0;CurrentVideoAnalyzed<ratFile.numberOfVideos();CurrentVideoAnalyzed++){
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
                
                if (PressedKey == 'N' || PressedKey == 'n')
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


                        std::ofstream ofs (ratFile.getOutputFilenameWidthPath().c_str(), std::ofstream::out);
                        ofs<<"VideoNumber,"<<"Frame,"<<"x1,"<<"y1,"<<"x2,"<<"y2,"<<"x3,"<<"y3,"<<"x4,"<<"y4,"<<"x5,"<<"y5,"<<"T1,"<<"T2,"<<"T3,"<<"T4,"<<"T5\n";


                        for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++){

                            for (int i=0;i<VideoToAnalyze[VideoNumber].NumberOfFrames;i++){
                                if (VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints>0){
                                    ofs<<VideoNumber<<",";
                                    ofs<<i<<",";
                                    for(int j=0; j< VideoToAnalyze[VideoNumber].FrameProperties[i].NumberOfTRegisteredPoints;j++){
                                        ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].CoorX<<",";
                                        ofs<<VideoToAnalyze[VideoNumber].FrameProperties[i].TrackedPointsInFrame[j].CoorY<<", ";
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
   const char *HEADER = "VideoNumber,Frame,x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,T1,T2,T3,T4,T5\n";
   std::ofstream ofs(ratFile.getOutputFilenameWidthPath().c_str(), std::ofstream::out);
   std::ofstream ofsCorrected(ratFile.getOutputFilenameCorrected().c_str());
   ofs          << HEADER;
   ofsCorrected << HEADER;
   for (int VideoNumber=0;VideoNumber<ratFile.numberOfVideos();VideoNumber++) {
       for (int i = 0 ;i < VideoToAnalyze[VideoNumber].NumberOfFrames; i++) {
           RatWalkFrameObject &frame = VideoToAnalyze[VideoNumber].FrameProperties[i];
           if (frame.NumberOfTRegisteredPoints > 0) {
               ofs << VideoNumber << "," << i;
               ofsCorrected << VideoNumber << "," << i;
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs << "," << frame.TrackedPointsInFrame[j].CoorX
                      << "," << frame.TrackedPointsInFrame[j].CoorY;
                  ofsCorrected << "," << frame.TrackedPointsInFrame[j].CoorXCorrected
                      << "," << frame.TrackedPointsInFrame[j].CoorYCorrected;
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs          << ",,";
                  ofsCorrected << ",,";
               }
               for(int j = 0; j < frame.NumberOfTRegisteredPoints; j++) {
                  ofs          << ',' << QString::number(frame.TrackedPointsInFrame[j].Theta, 'f', 10).toStdString();
                  ofsCorrected << ',' << QString::number(frame.TrackedPointsInFrame[j].ThetaCorrected, 'f', 10).toStdString();
               }
               for (int j = frame.NumberOfTRegisteredPoints; j < frame.NumberOfPointsToTrack; j++) {
                  ofs          << ',';
                  ofsCorrected << ',';
               }
               ofs          << "\n";
               ofsCorrected << '\n';
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
         currentFrame.NumberOfTRegisteredPoints = 0;
         for (int i = 0; i < prevFrame.NumberOfTRegisteredPoints; i++) {
            RatWalkControlPoint p = prevFrame.TrackedPointsInFrame[i];

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
   return ratFile.getVideoNames();
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
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, PointID, CurrentVideoAnalyzed);
   PointID = std::min(PointID+1, NpointsToTrack-1);
}

void RatWalkTracker::setPointOnCurrentFrame(int pointId, int x, int y,
                                            int frameWidth, int frameHeight) {
   RatWalkTrackerVideoObject &currentVideo = VideoToAnalyze[CurrentVideoAnalyzed];
   cv::Mat mat = getFrameWithRectangle();
   int x2 = mat.cols * x / frameWidth,
       y2 = mat.rows * y / frameHeight;
   currentVideo.SelectPoint(x2, y2, HalfWindowSize, pointId, CurrentVideoAnalyzed);
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

void RatWalkTracker::mouseHandlerForInitialTrackingPoints(int event, int x, int y, int flags) {
   if (event == CV_EVENT_LBUTTONDOWN)
   {

       VideoToAnalyze[CurrentVideoAnalyzed].SelectPoint(x, y, HalfWindowSize, PointID, CurrentVideoAnalyzed);
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
           circle(FrameWithRectangle, Point((int)round(VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].TrackedPointsInFrame[PointToShow].CoorX),(int)round(VideoToAnalyze[CurrentVideoAnalyzed].FrameProperties[VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrame].TrackedPointsInFrame[PointToShow].CoorY)),3, Scalar(0,0,255),CV_FILLED, 1,0);

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

       // TODO está función ya no sé usa en la implementación en Qt, tengo que
       // buscar dónde icorporar esto
       cv::Mat ImageToTransform=VideoToAnalyze[CurrentVideoAnalyzed].CurrentFrameData.clone();
       cv::Mat TransformedImage;
       // Use the Homography Matrix to warp the images
        if (CurrentVideoAnalyzed==0){
           cv::warpPerspective(ImageToTransform,TransformedImage,HLeft,cv::Size(TargetImage.cols,TargetImage.rows));
        }
        if (CurrentVideoAnalyzed==1){
           warpPerspective(ImageToTransform,TransformedImage,HMiddle,cv::Size(TargetImage.cols,TargetImage.rows));
        }
        if (CurrentVideoAnalyzed==2){
           warpPerspective(ImageToTransform,TransformedImage,HRight,cv::Size(TargetImage.cols,TargetImage.rows));
        }

        VideoToAnalyze[CurrentVideoAnalyzed].ShowSkeletonInCurrentTransformedFrame(TransformedImage);

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
