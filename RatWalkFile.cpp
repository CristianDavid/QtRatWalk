#include "RatWalkFile.h"

#include <string>
#include <vector>
#include <fstream>
#include <QFileInfo>
#include <QDir>

RatWalkFile::RatWalkFile(const char *filename):
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

const std::string &RatWalkFile::getVideoFilename(int idx) {
   return videoFilenames[idx];
}

const std::string &RatWalkFile::getOutputFilename() {
   return OutputFilename;
}

std::string RatWalkFile::getVideoFilenameWithPath(int idx) {
   return getProjectPath() + QDir::separator().toLatin1()
                           + getVideoFilename(idx);
}

std::string RatWalkFile::getOutputFilenameWidthPath() {
   return getProjectPath() + QDir::separator().toLatin1()
         + getOutputFilename();
}

std::string RatWalkFile::getOutputFilenameCorrected() {
   return getProjectPath() + QDir::separator().toLatin1()
         + OutputFilenameCorrected;
}

std::string RatWalkFile::getTargetFilename() {
   return getProjectPath() + QDir::separator().toLatin1()
         + "CalibrationLettersTarget.png";
}

std::string RatWalkFile::getProjectName() {
   return projectFileInfo.fileName().toStdString();
}

std::string RatWalkFile::getProjectPath() {
   return projectFileInfo.path().toStdString();
}

const std::vector<std::string> &RatWalkFile::getVideoNames() {
   return videoFilenames;
}

int RatWalkFile::numberOfVideos() {
   return videoFilenames.size();
}
