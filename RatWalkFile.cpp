#include "RatWalkFile.h"

#include <string>
#include <vector>
#include <fstream>
#include <QFileInfo>

RatWalkFile::RatWalkFile(const char *filename):
   projectFileInfo(filename) {
   std::string line;
   std::ifstream inFile(filename);
   if (inFile.is_open()) {
       while ( std::getline(inFile, line) ) {
          videoFilenames.push_back(line);
       }
       inFile.close();
   }
}

const std::string &RatWalkFile::getVideoFilename(int idx) {
   return videoFilenames[idx];
}

std::string RatWalkFile::getProjectName() {
   return projectFileInfo.fileName().toStdString();
}

std::string RatWalkFile::getProjectPath() {
   return projectFileInfo.path().toStdString();
}

int RatWalkFile::numberOfVideos() {
   return videoFilenames.size();
}
