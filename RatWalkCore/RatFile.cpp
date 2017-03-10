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

const char *RatFile::getVideoFilename(int idx, bool withPath) {
   if (idx < 0 || idx >= numberOfVideos())
      throw std::out_of_range("RatFile::getVideoFilename(int, bool)");
   std::string &res = withPath? videoFilenamesWithPath[idx] :
                                videoFilenames[idx];
   return res.c_str();
}

const char *RatFile::getOutputFilename() {
   return outputFilename.c_str();
}

const char *RatFile::getOutputFilenameWidthPath() {
   return outputFilenameWithPath.c_str();
}

const char *RatFile::getOutputFilenameCorrected() {
   return outputFilenameCorrectedWithPath.c_str();
}

const char *RatFile::getTargetFilename() {
   return targetFilenameWithPath.c_str();
}

const char *RatFile::getProjectName() {
   return projectName.c_str();
}

const char *RatFile::getProjectPath() {
   return projectPath.c_str();
}

const std::vector<std::string> &RatFile::getVideoFilenames() {
   return videoFilenames;
}

int RatFile::numberOfVideos() {
    return (int)videoFilenames.size();
}

const char *RatFile::getStepRegisterFilename(bool withPath) {
   const std::string &res = withPath? stepRegisterFilenameWithPath :
                                      stepRegisterFilename;
   return res.c_str();
}

} // namespace RatWalkCore
