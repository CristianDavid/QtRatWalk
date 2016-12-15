#include "RatWalkCore/RatFile.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <QDir>
#include <QFileInfo>

namespace RatWalkCore {

RatFile::RatFile(const char *filename):
   projectFileInfo(filename) {
   std::string line;
   std::ifstream inFile(filename);
   if (inFile.is_open()) {
       while (std::getline(inFile, line)) {
          videoFilenames.push_back(line);
       }
       inFile.close();
   }
   //Create the outputFile
   std::string delimiter = "+";
   std::string token =  videoFilenames[0].substr(0,  videoFilenames[0].find(delimiter));

   OutputFilename          = token + ".csv";
   OutputFilenameCorrected = token + "_Corrected.csv";
}

const std::string &RatFile::getVideoFilename(int idx) {
   if (idx < 0 || idx >= numberOfVideos())
      throw std::out_of_range("RatFile::getVideoFilename(int)");
   return videoFilenames[idx];
}

const std::string &RatFile::getOutputFilename() {
   return OutputFilename;
}

std::string RatFile::getVideoFilenameWithPath(int idx) {
   if (idx < 0 || idx >= numberOfVideos())
      throw std::out_of_range("RatFile::getVideoFilenameWithPath(int)");
   return getProjectPath() + QDir::separator().toLatin1()
                           + getVideoFilename(idx);
}

std::string RatFile::getOutputFilenameWidthPath() {
   return getProjectPath() + QDir::separator().toLatin1()
         + getOutputFilename();
}

std::string RatFile::getOutputFilenameCorrected() {
   return getProjectPath() + QDir::separator().toLatin1()
         + OutputFilenameCorrected;
}

std::string RatFile::getTargetFilename() {
   return getProjectPath() + QDir::separator().toLatin1()
         + "CalibrationLettersTarget.png";
}

std::string RatFile::getProjectName() {
   return projectFileInfo.fileName().toStdString();
}

std::string RatFile::getProjectPath() {
   return projectFileInfo.path().toStdString();
}

const std::vector<std::string> &RatFile::getVideoNames() {
   return videoFilenames;
}

int RatFile::numberOfVideos() {
   return (int)videoFilenames.size();
}

} // namespace RatWalkCore
