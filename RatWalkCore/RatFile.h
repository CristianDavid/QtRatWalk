#ifndef RATWALKCORE_RATFILE_H
#define RATWALKCORE_RATFILE_H

#include <string>
#include <vector>

namespace RatWalkCore {

/*!
 * \brief The RatFile class stores the information about a project defined
 *        by a .rat file.
 *
 * This class contains the routs and names of the files used in a project,
 * sucha as videos, ouput files, among others.
 */
class RatFile {
 public:
   /*!
    * \brief Creates a RatFile from a filename.
    *
    *  This constructor reads a file, ideally with the .rat extension, and
    *  gets the name of the videos contained in the project.
    *
    *  Each line of the input file must have the name of a video.
    *
    *  It also calculates the names of the output files and some others.
    *
    * \param[in] filename Name of the file with the project information.
    */
   RatFile(const char *filename);

   /*!
    * \brief getVideoFilename Gets the name of a video from the project.
    * \param[in] idx Index of the video.
    * \param[in] withPath Indicates if the filename should include the path.
    * \return A string with the name of the video, if withPath is true then the
    *         name includes the path of the file.
    * \throw std::out_of_range if the condition 0 <= idx < numberOfVideos() if
    *        is false.
    */
   const char *getVideoFilename(int idx, bool withPath = true);

   /*!
    * \brief getOutputFilename
    * \return The name of a csv file used to store the points and angles on
    *         each frame.
    */
   const char *getOutputFilename();

   /*!
    * \brief getOutputFilenameWidthPath
    * \see getProjectPath()
    * \see getOutputFilename()
    * \return Output filename including path.
    */
   const char *getOutputFilenameWidthPath();

   /*!
    * \brief getOutputFilenameCorrected
    * \return Filename of the corrected output.
    */
   const char *getOutputFilenameCorrected();

   /*!
    * \brief getTargetFilename
    * \return Name of the target image used for calibration.
    */
   const char *getTargetFilename();

   /*!
    * \brief getProjectName
    * \return The name of the project.
    */
   const char *getProjectName();

   /*!
    * \brief getProjectPath
    * \return Path of the project.
    */
   const char *getProjectPath();

   /*!
    * \brief getVideoFilenames
    * \return Vector with the names of the videos in the project.
    */
   const std::vector<std::string> &getVideoFilenames();

   /*!
    * \brief numberOfVideos() Number of videos in the project.
    * \see RatWalkFile()
    * \return Number of videos in the project.
    */
   int numberOfVideos();

   /*!
    * \brief getStepRegisterFilename
    * \param[in] withPath Indicates weather or not the result of the function
    *            should include the path of the file.
    * \return Name of the step register file.
    */
   const char *getStepRegisterFilename(bool withPath = true);

   const char *getCalibrationParametersFilename(bool withPath = true);
 private:
   std::vector<std::string> videoFilenames;
   std::vector<std::string> videoFilenamesWithPath;
   std::string projectPath,
               projectName,
               outputFilename,
               outputFilenameWithPath,
               outputFilenameCorrected,
               outputFilenameCorrectedWithPath,
               targetFilename,
               stepRegisterFilename,
               stepRegisterFilenameWithPath,
               calibrationParametersFilename,
               calibrationParametersFilenameWithPath;
};

} // namespace RatWalkCore

#endif // RATWALKCORE_RATFILE_H
