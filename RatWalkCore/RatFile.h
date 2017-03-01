#ifndef RATWALKFILE_H
#define RATWALKFILE_H

#include <vector>
#include <string>
#include <QFileInfo>

namespace RatWalkCore {

/*!
 * \brief La clase RatFile almacena la información
 *        sobre un proyecto especificado en un archivo .rat.
 *
 * Esta clase contiene las rutas y los nombres de los archivos utilizados
 * en un proyecto, nombre de videos, archivos de salida, etc.
 */
class RatFile {
 public:
   /*!
    * \brief RatFile Construye a partir de un nombre de archivo.
    *
    *  Este constructor lee un archivo, idealmente con la extensión .rat, y
    *  obtiene los nombres de los vídeos que forman el proyecto.
    *
    *  Cada línea del archivo de entrada debe tener el nombre de un video.
    *
    * \see getVideoFilenameWithPath(int)
    * \see getVideoFilename(int)
    *
    * \param[in] filename el nombre del archivo con la información del proyecto.
    */
   RatFile(const char *filename);

   /*!
    * \brief getVideoFilename regresa el nombre de un video del proyecto
    * \param[in] idx el índice del video
    * \return Una cadena con el nombre del video.
    * \throw std::out_of_range si no se cumple que 0 <= idx < numberOfVideos()
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
    * \throw std::out_of_range si no se cumple que 0 <= idx < numberOfVideos()
    */
   std::string getVideoFilenameWithPath(int idx);

   /*!
    * \brief getOutputFilenameWidthPath
    * \see getProjectPath()
    * \see getOutputFilename()
    * \return Nombre del archivo de salida con ruta absoluta
    */
   std::string getOutputFilenameWidthPath();

   /*!
    * \brief getOutputFilenameCorrected
    * \return Nombre del archivo de salida corregida con ruta absoluta
    */
   std::string getOutputFilenameCorrected();

   /*!
    * \brief getTargetFilename
    * \return Nombre de la imagen target
    * \todo Que la imagen deba estar en la misma ruta que el ejecutable
    */
   std::string getTargetFilename();

   /*!
    * \brief getProjectName
    * \return El nombre del archivo con la información del proyecto. Es el mismo
    *         que el parámetro pasado a RatFile()
    */
   std::string getProjectName();


   /*!
    * \brief getProjectPath
    * \return Ruta del proyecto
    */
   std::string getProjectPath();

   /*!
    * \brief getVideoNames
    * \return Vector con los nombres de los videos del proyecto
    */
   const std::vector<std::string> &getVideoNames();

   /*!
    * \brief numberOfVideos() Número de vídeos en el proyecto.
    * \see RatWalkFile()
    * \return Entero con la cantidad de vídeos que conforman el proyecto.
    */
   int numberOfVideos();

   std::string getStepRegisterFilename();
 private:
   QFileInfo projectFileInfo;
   std::vector<std::string> videoFilenames;
   std::string OutputFilename;
   std::string OutputFilenameCorrected;
};

} // namespace RatWalkCore

#endif // RATWALKFILE_H
