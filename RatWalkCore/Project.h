#ifndef RATWALKCORE_PROJECT_H
#define RATWALKCORE_PROJECT_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "RatWalkCore/Video.h"
#include "RatWalkCore/RatFile.h"
#include "RatWalkCore/StepRegister.h"
#include "RatWalkCore/Corrector.h"

namespace RatWalkCore {

/*!
 * \brief The class Project represents an open RatWalk project, it handles the
 *        videos, frames and points independently from the gui.
 */
class Project {
 public:
   /*!
    * \brief Creates a new project from a rat file
    * \param[in] fileName Filename of the project's rat file.
    */
   Project(const char *fileName);

   /*!
    * \brief addPointOnCurrentFrame Adds a new point on the current frame
    *
    * This methd adds a new ControPoint in the current frame. The point (x, y)
    * should be withing the limits of the size of the frame shown on the
    * screen, this method translated the input coordinates to the actual size
    * of the frame.
    *
    * \param[in] x X coordinate of the point.
    * \param[in] y Y coordinate of the point.
    * \param[in] frameWidth Width of the current frame shown on the screen.
    * \param[in] frameHeight Height of the current frame shown on the screen.
    */
   void addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight);

   /*!
    * \brief setPointOnCurrentFrame Sets a point on the coordinates (x, y)
    * \param[in] pointId Id of the point to be set
    * \param[in] x X coordinate of the point
    * \param[in] y Y coordinate of the point
    * \param[in] frameWidth Witdth of the frame shown on the screen
    * \param[in] frameHeight Height of the frame shown on the screen
    */
   void setPointOnCurrentFrame(int pointId, int x, int y,
                               int frameWidth, int frameHeight);

   /*!
    * \brief deletePointOnCurrentFrame Deletes a point from the current frame.
    * \param[in] pointId If of the point to be deleted.
    */
   void deletePointOnCurrentFrame(int pointId);

   /*!
    * \brief Gets the id of the closest point to (x, y).
    *
    * This function is used to be able to select a point with the cursor by
    * choosing the closest point that is also close enough to the cursor
    * position.
    *
    * \param[in] x X coordinate.
    * \param[in] y Y coordinate.
    * \param[in] frameWidth  Width of the current frame as shown on the screen.
    * \param[in] frameHeight Height of the current frame as shown on the screen.
    * \param[in] minDistance Minimum acceptable distance to choose a point.
    *
    * \return Id of the closest point to (x, y) that meets the condition of
    *         beeing at a minimum distance minDistance from (x, y), or -1
    *         if such point doesn't exist.
    */
   int getClosestPointID(int x, int y, int frameWidth, int frameHeight,
                         double minDistance = 1.0);

   /*!
    * \brief setFrame Sets the current frame
    * \param[in] Position Position of the new current frame
    */
   void setFrame(int Position);

   /*!
    * \brief nextFrame moves the current frame to the next position.
    */
   void nextFrame();

   /*!
    * \brief prevFrame moves the current frame to the previous position
    */
   void prevFrame();

   /*!
    * \brief Saves the captured points and steps to disk.
    */
   void save();

   /*!
    * \brief Brings the captured points from a previous
    *        frame to the current one.
    *
    * This method copies the points from the last captured frame onto the
    * current one, if no points have been captured before on any previous frame
    * a default set of points is given.
    */
   void bringPreviousSkeleton();

   /*!
    * \brief Gets the current frame as a matrix.
    * \return OpenCv matrix containing the values of each pixel on the frame.
    */
   cv::Mat getFrameWithRectangle();

   /*!
    * \brief Gets the current frame as a matrix, with the skeleton on it.
    * \return OpenCv matrix with the current frame including the skeleton.
    */
   cv::Mat getFrameWithSkeleton();

   /*!
    * \brief Gets a zoomed region of the current frame
    *
    * Gets a rectangular region of the current frame with its center at (x, y),
    * this region is used to implement a zoom functionality.
    *
    * \param[in] x X coordinate of the region's center.
    * \param[in] y Y coordinate of the region's center.
    * \param[in] frameWidth  Width of the current frame as shown on the screen.
    * \param[in] frameHeight Height of the current frame as shown on the screen.
    * \return Matrix with the selected region of the frame.
    */
   cv::Mat getZoomedRegion(int x, int y, int frameWidth, int frameHeight);

   /*!
    * \brief getCurrentVideoAnalyzed
    * \return Reference to the current video.
    */
   const Video &getCurrentVideoAnalyzed();

   /*!
    * \brief getVideoFilenames
    * \return Vector of string with the names of the videos
    */
   const std::vector<std::string> &getVideoFilenames();

   /*!
    * \brief Sets the video thats is beeing currently analyzed.
    * \param[in] index Position of the new video to analyze.
    */
   void setCurrentVideo(int index);

   /*!
    * \brief Saves the annotated corrected data to disk.
    *
    * Saves the corrected coordinates and angles to a csv file.
    */
   void saveCorrectedFile();

   /*!
    * \brief Produces a vector of videos in the rat project
    * \return Reference to a vector containing all of the videos.
    */
   std::vector<Video> &getVideos();

   /*!
    * \brief getStepRegisters
    * \return Reference to a vector containing the step register of each video
    *         on this project.
    */
   std::vector<StepRegister> &getStepRegisters();

   /*!
    * \brief getCurrentStepRegister
    * \return A reference to the step register of the current video.
    */
   StepRegister &getCurrentStepRegister();

   /*!
    * \brief getCurrentVideoIndex
    * \return Index of the current video.
    */
   int getCurrentVideoIndex();

   /*!
    * \brief loadStepRegister Loads the step register from a csv file.
    * \param filename
    */
   void loadStepRegister(const char *filename);

   /*!
    * \brief saveStepRegister Saves the step register to a csv file.
    * \param filename
    */
   void saveStepRegister(const char *filename);

   /*!
    * \brief getProjectName
    * \return String containing the name of the project.
    */
   const char *getProjectName();

   /*!
    * \brief getSize
    * \return Number of videos in the project.
    */
   int getSize();

 private:
   std::vector<Video> VideoToAnalyze;
   std::vector<StepRegister> stepRegisters;
   int CurrentVideoAnalyzed = 0;
   int PointID = 0;
   RatFile ratFile;
   Corrector corrector;
};

} // namespace RatWalkCore

#endif // RATWALKCORE_PROJECT_H
