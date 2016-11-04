#ifndef RATWALKFILE_H
#define RATWALKFILE_H

#include <vector>
#include <string>
#include <QFileInfo>

class RatWalkFile {
 public:
   RatWalkFile(const char *filename);
   const std::string &getVideoFilename(int idx);
   std::string getProjectName();
   std::string getProjectPath();
   int numberOfVideos();
 private:
   QFileInfo projectFileInfo;
   std::vector<std::string> videoFilenames;
   std::string outputFilename;
};

#endif // RATWALKFILE_H
