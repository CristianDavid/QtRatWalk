#include "Corrector.h"

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

namespace RatWalkCore {

Corrector::Corrector(std::vector<Video> &VideoToAnalyze)
 : VideoToAnalyze(VideoToAnalyze) {
}

cv::Mat Corrector::performCorrection(std::string targetFilename, int MinHessian) {
   ////////////
   //PERFORM THE CORRECTION OF THE VIDEOS
   /////////////
   cv::Mat OriginalTargetImage= cv::imread(targetFilename);
   //Mat OriginalTargetImage= imread( "CalibrationShoes.png");
   double Scale=480/(double)OriginalTargetImage.rows;
   cv::resize(OriginalTargetImage, TargetImage, cv::Size(0,0),Scale,Scale,cv::INTER_LINEAR);
   cvtColor(TargetImage, TargetImageGrayG, CV_RGB2GRAY);

   Image1=VideoToAnalyze[0].CurrentFrameData.clone(); //! \todo Está casado con que sean tres vídeos
   Image2=VideoToAnalyze[1].CurrentFrameData.clone();
   Image3=VideoToAnalyze[2].CurrentFrameData.clone();


   //Convert the images to grayScale
   std::cout<<"Convert the images to grayScale";

   cvtColor(Image1, ImageLeftG, CV_RGB2GRAY);
   cvtColor(Image2, ImageMiddleG, CV_RGB2GRAY);
   cvtColor(Image3, ImageRightG, CV_RGB2GRAY);

   int blursize=1;
   blur(TargetImageGrayG, TargetImageGray, cv::Size(blursize,blursize));
   blur(ImageLeftG, ImageLeft, cv::Size(blursize,blursize));
   blur(ImageMiddleG, ImageMiddle, cv::Size(blursize,blursize));
   blur(ImageRightG, ImageRight, cv::Size(blursize,blursize));

   //Detect KeyPoints using SURF detector
   std::cout<<"Detect KeyPoints using SURF detector";
   cv::SurfFeatureDetector detector(MinHessian);
   //cv::SiftFeatureDetector detector;
   std::vector<cv::KeyPoint> keyPointsImageLeft, KeyPointsImageMiddle, KeyPointsImageRight, KeyPointsTargetImage;

   detector.detect(ImageLeft, keyPointsImageLeft);
   detector.detect(ImageMiddle, KeyPointsImageMiddle);
   detector.detect(ImageRight, KeyPointsImageRight);
   detector.detect(TargetImageGray, KeyPointsTargetImage);

   //Calculate descriptors (feature vectors)
   std::cout<<"Calculate descriptors (feature vectors)";
   cv::SurfDescriptorExtractor extractor;
   //cv::SiftDescriptorExtractor extractor;
   cv::Mat DescriptorsImageLeft, DescriptorsImageRight, DescriptorsImageMidde,DescriptorsTargetImage;

   extractor.compute(ImageLeft, keyPointsImageLeft, DescriptorsImageLeft);
   extractor.compute(ImageMiddle, KeyPointsImageMiddle, DescriptorsImageMidde);
   extractor.compute(ImageRight, KeyPointsImageRight, DescriptorsImageRight);
   extractor.compute(TargetImageGray, KeyPointsTargetImage, DescriptorsTargetImage);

   //Maching descriptor vectors using FLANN matcher
   std::cout<<"Maching descriptor vectors using FLANN matcher";
   cv::FlannBasedMatcher matcher;
   std::vector<cv::DMatch> MatchesImageLeft,MatchesImageMiddle,MatchesImageRight;

   matcher.match(DescriptorsImageLeft, DescriptorsTargetImage, MatchesImageLeft);
   matcher.match(DescriptorsImageMidde, DescriptorsTargetImage, MatchesImageMiddle);
   matcher.match(DescriptorsImageRight, DescriptorsTargetImage, MatchesImageRight);

   //Quick calculation of max and min distances between keypoints
   std::cout<<"Quick calculation of max and min distances between keypoints";
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
   std::cout<<"Keep only the good matches";
   //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
   std::vector<cv::DMatch> good_matchesLeft,good_matchesMiddle,good_matchesRight;

   for( int i = 0; i < DescriptorsImageLeft.rows; i++ ){
      if( MatchesImageLeft[i].distance < 3*min_distLeft ){
         good_matchesLeft.push_back( MatchesImageLeft[i]);
      }
   }
   for( int i = 0; i < DescriptorsImageMidde.rows; i++ ){
      if( MatchesImageMiddle[i].distance < 3*min_distMiddle ){
         good_matchesMiddle.push_back( MatchesImageMiddle[i]);
      }
   }
   for( int i = 0; i < DescriptorsImageRight.rows; i++ ){
      if( MatchesImageRight[i].distance < 3*min_distRight ){
         good_matchesRight.push_back( MatchesImageRight[i]);
      }
   }

   cv::Mat img_matches;
   cv::drawMatches(ImageMiddle , KeyPointsImageMiddle, TargetImageGray, KeyPointsTargetImage,
                   good_matchesMiddle, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                   std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

   //  imshow("MatchesLeft",img_matches);
   //  waitKey(1);

   //-- Compute the Trasnformation Matrix
   std::vector<cv::Point2f> ImageLeftControlPoints,ImageRightControlPoints,ImageMiddleControlPoints;
   std::vector<cv::Point2f> TargetImageControlPointsLeft,TargetImageControlPointsMiddle,TargetImageControlPointsRight;
   for( int i = 0; i < (int)good_matchesLeft.size(); i++ ){
      //-- Get the keypoints from the good matches
      ImageLeftControlPoints.push_back( keyPointsImageLeft[ good_matchesLeft[i].queryIdx ].pt );
      TargetImageControlPointsLeft.push_back( KeyPointsTargetImage[ good_matchesLeft[i].trainIdx ].pt );
   }
   HLeft = findHomography( ImageLeftControlPoints, TargetImageControlPointsLeft, CV_RANSAC );

   for( int i = 0; i < (int)good_matchesMiddle.size(); i++ ) {
      //-- Get the keypoints from the good matches
      ImageMiddleControlPoints.push_back( KeyPointsImageMiddle[ good_matchesMiddle[i].queryIdx ].pt );
      TargetImageControlPointsMiddle.push_back( KeyPointsTargetImage[ good_matchesMiddle[i].trainIdx ].pt );
   }
   HMiddle = findHomography( ImageMiddleControlPoints, TargetImageControlPointsMiddle, CV_RANSAC );

   for( int i = 0; i < (int)good_matchesRight.size(); i++ ) {
      //-- Get the keypoints from the good matches
      ImageRightControlPoints.push_back( KeyPointsImageRight[ good_matchesRight[i].queryIdx ].pt );
      TargetImageControlPointsRight.push_back( KeyPointsTargetImage[ good_matchesRight[i].trainIdx ].pt );
   }
   HRight = findHomography( ImageRightControlPoints, TargetImageControlPointsRight, CV_RANSAC );

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

   cv::Mat FinalResutl(TargetImage.rows,TargetImage.cols, CV_8UC3, cv::Scalar(0, 0, 0));

   for(int i = 0; i < FinalResutl.rows; i++){
      for(int j = 0; j < FinalResutl.cols; j++){
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
   return FinalResutl;
}

bool Corrector::loadCalibrationParametersFromFile(const char *filename) {
   cv::FileStorage storage(filename, cv::FileStorage::READ);
   if (storage.isOpened()) {
      storage["HLeft"]   >> HLeft;
      storage["HMiddle"] >> HMiddle;
      storage["HRight"]  >> HRight;
      return true;
   } else {
      return false;
   }
}

bool Corrector::saveCalibrationParametersToFile(const char *filename) {
   cv::FileStorage storage(filename, cv::FileStorage::WRITE);
   if (storage.isOpened()) {
      storage << "HLeft"   << HLeft;
      storage << "HMiddle" << HMiddle;
      storage << "HRight"  << HRight;
      return true;
   } else {
      return false;
   }
}

cv::Mat Corrector::getHLeft() {
   return HLeft;
}

cv::Mat Corrector::getHMiddle() {
   return HMiddle;
}

cv::Mat Corrector::getHRight() {
   return HRight;
}

} // namespace RatWalkCore
