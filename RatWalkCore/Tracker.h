#ifndef RATWALKTRACKER_H
#define RATWALKTRACKER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "RatWalkCore/Video.h"
#include "RatWalkCore/RatFile.h"
#include "RatWalkCore/StepRegister.h"

#define HalfWindowSize 9
#define NpointsToTrack 5

namespace RatWalkCore {

/*!
 * \brief La clase Tracker maneja toda la aplicación y los datos de manera
 * independiente de la interfaz de usuario
 */
class Tracker {
 public:
   /*!
    * \brief Tracker Constructor con el nombre de archivo rat
    * \param fileName Nombre del archivo con el proyecto rat
    * \todo Hacer mejor manejo de errores al momento de leer archivos
    */
   Tracker(const char *fileName);

   /*!
    * \brief addPointOnCurrentFrame Agrega un nuevo punto en el frame actual
    *
    * Este método agrega un nuevo punto en el frame actual. El punto (x, y)
    * que se recibe en los parámetros debe estar de acuerdo al tamaño con que el
    * frame se muestra en pantalla, esta función traduce el punto a las
    * coordenadas reales del frame.
    *
    * \param x Coordenada en x del punto
    * \param y Coordenada en y del punto
    * \param frameWidth Ancho con el que se está mostrando el frame actual
    * \param frameHeight Altura con la que se está mostrando el frame actual
    *
    * \see addPointOnCurrentFrame()
    *
    * \todo Validar que (x, y) esté dentro del rango válido
    * \todo validar que frameWidth  > 0
    * \todo validar que frameHeight > 0
    * \todo Adaptar a cambios en las interfaces de las otras clases
    */
   void addPointOnCurrentFrame(int x, int y, int frameWidth, int frameHeight);


   /*!
    * \brief setPointOnCurrentFrame Mueve un punto a las posiciones x, y
    * \param pointId Id del punto a mover
    * \param x Coordenada del punto en x
    * \param y Coordenada del punto en y
    * \param frameWidth Ancho con el que se está mostrando el frame actual
    * \param frameHeight Altura con la que se está mostrando el frame actual
    *
    * \todo Hacer validaciones de los parámetros de entrada
    * \todo Adaptar a cambios a las interfaces de las otras clases
    */
   void setPointOnCurrentFrame(int pointId, int x, int y,
                               int frameWidth, int frameHeight);

   /*!
    * \brief deletePointOnCurrentFrame Elimina un punto en el frame actual
    * \param[in] pointId Posición del punto a ser eliminado
    */
   void deletePointOnCurrentFrame(int pointId);

   /*!
    * \brief Obtiene el id del punto más cercano a (x, y)
    *
    * Esta función se utiliza para poder seleccionar un punto con el cursor,
    * de tal manera que se selecciona el punto más cercano que cumpla con la
    * condición de estar lo bastante cerca.
    *
    * \param x Coordenada en x
    * \param y Coordenada en y
    * \param frameWidth  Ancho con que se muestra el frame actual
    * \param frameHeight Altura con que se muestra el frame actual
    * \param minDistance Mínima distancia a la que debe estar el punto seleccionado
    *
    * \return Id del punto más cercano a (x, y) que cumpla con la condición
    *         de estar a una distancia mínima de minDistance, o -1 si tal punto
    *         no existe
    * \todo Validar parametros de entrada
    */
   int getClosestPointID(int x, int y, int frameWidth, int frameHeight,
                         double minDistance = 1.0);

   /*!
    * \brief setFrame Cambia el frame actual a la posición indicada
    * \param[in] Position Posición del frame
    * \todo validar posición
    */
   void setFrame(int Position);

   /*!
    * \brief nextFrame, mueve el frame actual a la siguiente posición
    */
   void nextFrame();

   /*!
    * \brief prevFrame, mueve el frame actual a la posición previa
    */
   void prevFrame();

   /*!
    * \brief Guarda los puntos capturados en un archivo
    */
   void guardar();

   /*!
    * \brief Trae los punto del último frame capturado al frame actual.
    *
    * Esta función copia los puntos del último frame que se haya capturado
    * al frame actual, en caso de que no se haya capturado ningún frame
    * anterior, se establece en esqueleto por defecto.
    *
    */
   void traeEsqueleto();

   /*!
    * \brief Obtiene el frame actual en forma de matrix
    * \return Matriz de OpenCv con los valores de cada pixel del frame
    */
   cv::Mat getFrameWithRectangle();

   /*!
    * \brief Obtiene el frame actual con esqueleto
    * \return Matriz de OpenCv con el esqueleto en el frame
    */
   cv::Mat getFrameWithSkeleton();

   /*!
    * \brief Obtiene una region del frame con zoom
    *
    * Obtiene una región rectangular cuyo centro es (x, y) y dicha región
    * se utiliza para ser mostrada con zoom
    *
    * \param x Posición en x
    * \param y Posición en y
    * \param frameWidth  Ancho con que se muestra el frame actual
    * \param frameHeight Alto con que se muestra el frame actual
    * \return Matriz con la región deseada
    */
   cv::Mat getZoomedRegion(int x, int y, int frameWidth, int frameHeight);

   /*!
    * \brief getCurrentVideoAnalyzed
    * \return Referencia al video actual
    */
   const Video &getCurrentVideoAnalyzed();

   /*!
    * \brief getVideoFilenames
    * \return Vector de cadenas con los nombres de los videos
    */
   const std::vector<std::string> &getVideoFilenames();

   /*!
    * \brief Establece el video que se está analizando actualmente
    * \param[in] index Posición del video a analizar
    * \todo validar índice
    */
   void setCurrentVideo(int index);

   /*!
    * \brief Saves the annotated corrected data to disk.
    *
    * Saves the corrected coordinates and angles to a csv file.
    *
    */
   void saveCorrectedFile();

   /*!
    * \brief Produces an array of videos in the rat project
    * \return Array containing all of the videos
    */
   Video *getVideos();

   StepRegister *getStepRegisters();

   StepRegister &getCurrentStepRegister();

   int getCurrentVideoIndex();

   void loadStepRegister(const char *filename);
   void saveStepRegister(const char *filename);

   std::string getProjectName();

 private:
   cv::Mat TargetImage, TargetImageGray,
           Image1,     Image2,       Image3,
           ImageLeft,  ImageMiddle,  ImageRight,
           ImageLeftG, ImageMiddleG, ImageRightG, TargetImageGrayG;
   Video VideoToAnalyze[3];
   int CurrentVideoAnalyzed = 0;
   int PointID = 0;
   RatFile ratFile;
   StepRegister stepRegisters[3];
};

} // namespace RatWalkCore

#endif // RATWALKTRACKER_H
