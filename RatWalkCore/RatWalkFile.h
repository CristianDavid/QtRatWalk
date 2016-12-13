#ifndef RATWALKFILE_H
#define RATWALKFILE_H

#include <vector>
#include <string>
#include <QFileInfo>

/*!
 * \brief La clase RatWalkFile almacena la información
 *        sobre un proyecto especificado en un archivo .rat.
 *
 * Esta clase contiene las rutas y los nombres de los archivos utilizados
 * en un proyecto, nombre de videos, archivos de salida, etc.
 */
class RatWalkFile {
 public:
   /*!
    * \brief RatWalkFile Construye a partir de un nombre de archivo.
    *
    *  Este constructor lee un archivo, idealmente con la extensión .rat, y
    *  obtiene los nombres de los vídeos que forman el proyecto.
    *
    *  Cada línea del archivo de entrada debe tener el nombre del un video.
    *
    * \see getVideoFilenameWithPath(int)
    * \see getVideoFilename(int)
    *
    * \param filename el nombre del archivo con la información del proyecto.
    */
   RatWalkFile(const char *filename);

   /*!
    * \brief getVideoFilename regresa el nombre de un video del proyecto
    * \param[in] idx el índice del video, 0 <= idx < numberOfVideos()
    * \return Una cadena con el nombre del video.
    */
   const std::string &getVideoFilename(int idx);

   /*!
    * \brief getOutputFilename
    * \return El nombre del archivo de salida, generalmente un .csv
    */
   const std::string &getOutputFilename();

   /*!
    * \brief getVideoFilenameWithPath
    *
    * Regresa el nombre de un video del proyecto incluyendo la ruta del video.
    * Equivalente a getProyectPath() + getVideoFilename()
    *
    * \see getProjectPath()
    * \see getVideoFilename()
    *
    * \param[in] idx el índice del video, 0 <= idx < numberOfVideos()
    * \return nombre del archivo de video con ruta absoluta
    */
   std::string getVideoFilenameWithPath(int idx);

   /*!
    * \brief getOutputFilenameWidthPath
    * \see getProjectPath()
    * \see getOutputFilename()
    * \return Nombre del archivo de salida con ruta absoluta
    */
   std::string getOutputFilenameWidthPath();
   std::string getOutputFilenameCorrected();
   std::string getTargetFilename();

   /*!
    * \brief getProjectName
    * \return El nombre del archivo con la información del proyecto. Es el mismo
    *         que el parámetro pasado a RatWalkFile()
    */
   std::string getProjectName();
   std::string getProjectPath();
   const std::vector<std::string> &getVideoNames();
   /*!
    * \brief numberOfVideos() Número de vídeos en el proyecto.
    * \see RatWalkFile()
    * \return Entero con la cantidad de vídeos que conforman el proyecto.
    */
   int numberOfVideos();
 private:
   QFileInfo projectFileInfo;
   std::vector<std::string> videoFilenames;
   std::string OutputFilename;
   std::string OutputFilenameCorrected;
};

#endif // RATWALKFILE_H
