#ifndef RATWALKFILE_H
#define RATWALKFILE_H

#include <vector>
#include <string>
#include <QFileInfo>

class RatWalkFile {
 public:
   RatWalkFile(const char *filename);
   const std::string &getVideoFilename(int idx);
   const std::string &getOutputFilename();
   std::string getVideoFilenameWithPath(int idx);
   std::string getOutputFilenameWidthPath();
   std::string getOutputFilenameCorrected();
   std::string getTargetFilename();
   std::string getProjectName();
   std::string getProjectPath();
   const std::vector<std::string> &getVideoNames();
   int numberOfVideos();
 private:
   QFileInfo projectFileInfo;
   std::vector<std::string> videoFilenames;
   std::string OutputFilename;
   std::string OutputFilenameCorrected;
};

#endif // RATWALKFILE_H
