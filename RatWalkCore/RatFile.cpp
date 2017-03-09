#include "RatWalkCore/RatFile.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <QDir>
#include <QFileInfo>

const char *RESOURCES_FOLDER = "resources";

namespace RatWalkCore {

RatFile::RatFile(const char *filename) {
   QFileInfo   projectFileInfo(filename);
   std::string dirSeparator = {QDir::separator().toLatin1()};
   projectPath = projectFileInfo.path().toStdString() + dirSeparator;
   projectName = projectFileInfo.fileName().toStdString();
   outputFilename          = projectName + ".csv";
   outputFilenameCorrected = projectName + "_Corrected.csv";
   targetFilename          = RESOURCES_FOLDER + dirSeparator +
                              "CalibrationLettersTarget.png";
   stepRegisterFilename    = projectName + "StepRegister.csv";
   outputFilenameWithPath          = projectPath + outputFilename;
   outputFilenameCorrectedWithPath = projectPath + outputFilenameCorrected;
   targetFilenameWithPath          = projectPath + targetFilename;
   stepRegisterFilenameWithPath    = projectPath + stepRegisterFilename;

   std::ifstream inFile(filename);
   std::string line;
   if (inFile.is_open()) {
       while (std::getline(inFile, line)) {
          videoFilenames.push_back(line);
          videoFilenamesWithPath.push_back(projectPath + line);
       }
       inFile.close();
   }
}

const std::string &RatFile::getVideoFilename(int idx) {
   if (idx < 0 || idx >= numberOfVideos())
      throw std::out_of_range("RatFile::getVideoFilename(int)");
   return videoFilenames[idx];
}

const std::string &RatFile::getOutputFilename() {
   return outputFilename;
}

std::string RatFile::getVideoFilenameWithPath(int idx) {
   if (idx < 0 || idx >= numberOfVideos())
      throw std::out_of_range("RatFile::getVideoFilenameWithPath(int)");
   return videoFilenamesWithPath[idx];
}

std::string RatFile::getOutputFilenameWidthPath() {
   return outputFilenameWithPath;
}

std::string RatFile::getOutputFilenameCorrected() {
   return outputFilenameCorrectedWithPath;
}

std::string RatFile::getTargetFilename() {
   return targetFilenameWithPath;
}

std::string RatFile::getProjectName() {
   return projectName;
}

std::string RatFile::getProjectPath() {
   return projectPath;
}

const std::vector<std::string> &RatFile::getVideoNames() {
   return videoFilenames;
}

int RatFile::numberOfVideos() {
    return (int)videoFilenames.size();
}

std::string RatFile::getStepRegisterFilename() {
    return stepRegisterFilenameWithPath;
}

} // namespace RatWalkCore
