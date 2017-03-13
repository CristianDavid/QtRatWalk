#ifndef ANGLEPLOTTER_H
#define ANGLEPLOTTER_H

#include <vector>

#include <QWidget>
#include <QPoint>

#include <RatWalkGui/Plotter.h>

namespace RatWalkGui {

namespace Ui {
class AnglePlotter;
}

/*!
 * \brief The AnglePlotter class uses a RatWalkGui::Plotter to plot the angles
 * calculated on each frame.
 *
 * This class is used to plot the angles calculated on each frame, here the
 * x-axis represets the frame and the y-axis represents the value of the angle.
 */
class AnglePlotter : public QWidget {
    Q_OBJECT
public:
   /*!
    * \brief AnglePlotter Creates a new AnglePlotter
    * \param parent
    */
   explicit AnglePlotter(QWidget *parent = 0);

   /*!
    * \brief Destroys the angle plotter
    */
   ~AnglePlotter();

   /*!
    * \brief setFramesPerVideo Sets the number of frames that are plotted
    * for each video.
    *
    * This method dived the x axis of the plotter in framesPerVideo.size()
    * sections, each of the represents exaclty one video. The size of each
    * section is proportional to the number of frames on it.
    *
    * \param[in] framesPerVideo Vector containing the number of frames of
    * each vidoe.
    */
   void setFramesPerVideo(std::vector<int> &framesPerVideo);

   /*!
    * \brief addStep Adds a new step that should be drawn.
    *
    * This function adds a new step to the plotter, steps are drawn as a
    * semi-transparent pink area on the window.
    *
    * \param[in] video Video number.
    * \parav[in] beginFrame Begining frame in relation to video.
    * \param[in] endFrame Ending frame in relation to video.
    */
   void addStep(int video, int beginFrame, int endFrame);

   /*!
    * \brief clearSteps Ereases all of the steps on the plotter.
    */
   void clearSteps();

   /*!
    * \brief getPlotter Gets internal RatWalkCore::Plotter used by this object.
    * \return The internal RatWalkCore::Plotter used by this object.
    */
   Plotter *getPlotter();
protected:
   bool eventFilter(QObject *watched, QEvent *event);
private slots:
   void on_toolBtnZoomIn_clicked();
   void on_toolBtnZoomOut_clicked();
   void on_toolBtnZoomFitBest_clicked();
private:
   void showCurrentCoordinates(QPoint realPoint);
   Ui::AnglePlotter *ui;
   std::vector<int> framesPerVideo;
};

} // namespace RatWalkGui

#endif // ANGLEPLOTTER_H
