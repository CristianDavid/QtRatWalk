#ifndef RATWALKGUI_MAINWINDOW_H
#define RATWALKGUI_MAINWINDOW_H

#include <array>
#include <memory>
#include <vector>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QStatusBar>
#include "RatWalkGui/AnglePlotter.h"
#include "RatWalkCore/Constantes.h"
#include "RatWalkGui/ImageViewer.h"
#include "RatWalkCore/Project.h"

namespace RatWalkGui {

namespace Ui {
class MainWindow;
}

/*!
 * \brief The MainWindow class is the main window of the QtRatWalk application.
 */
class MainWindow : public QMainWindow {
   Q_OBJECT

public:
   /*!
    * \brief Creates a new MainWindow
    * \param parent The widget's parent.
    */
   explicit MainWindow(QWidget *parent = 0);

   /*!
    * Destroys the MainWindow.
    */
   ~MainWindow();

   /*!
    * \brief reloadFrame reloads the content of the current frame shown on the
    *        screen.
    */
   void reloadFrame();

   /*!
    * \brief showZoomedRegion Shows the zoomed region.
    * \param point The center of the region to show, usually the cursor
    *        position.
    * \param frameWidth
    * \param frameHeight
    */
   void showZoomedRegion(QPoint point, int frameWidth, int frameHeight);
protected:
   bool eventFilter(QObject *watched, QEvent *event);

private:
   void mousePressEventOnPnlFrame(QMouseEvent *event);

   void mouseReleaseEventOnPnlFrame(QMouseEvent *event);

   void mouseMoveEventOnPnlFrame(QMouseEvent *event);

   int pointToGrabId(QPoint pos, double radius);

private slots:
   void onActionsShowSubWindowTriggered();

   void on_actionOpen_triggered();

   void on_actionSave_triggered();

   void on_actionClose_triggered();

   void on_btnNext_clicked();

   void on_btnPrev_clicked();

   void on_btnTraerEsqueleto_clicked();

   void on_checkBoxMostrarEsqueleto_stateChanged(int state);

   void on_horizontalSlider_valueChanged(int value);

   void on_spinBoxCambiarFrame_valueChanged(int value);

   void on_twProjecto_doubleClicked(const QModelIndex &index);

   void on_actionDelete_point_triggered();

   void on_btnStartStep_clicked();

   void on_btnFinishStep_clicked();

   void on_btnDiscardStep_clicked();

   void on_btnEreaseStep_clicked();

   void on_actionExport_angles_triggered();

   void on_actionMostrar_zoom_triggered();

private:
   typedef std::shared_ptr<RatWalkCore::Project> ProjectPtr;
   typedef std::vector<ProjectPtr> ProjectVector;
   typedef std::array<AnglePlotter*, RatWalkCore::NUMBER_OF_ANGLES_CALCULATED>
      AnglePlotterArray;

   void onFrameNumberChanged();
   void updateStepInfo();
   void loadSteps();
   void setCurrentProject(int projectIdx);
   ProjectPtr getCurrentProject();
   AnglePlotterArray &getAnglePlotters();

   Ui::MainWindow *ui;
   ProjectVector projects;
   int currentProjectIdx;
   ImageViewer *zoomedRegionWindow;
   QPoint imageViewerClickedPos;
   int grabbedPointId;
   int stepBegin = -1;
   std::vector<AnglePlotterArray> anglePlotters;
   std::array<QMdiSubWindow, RatWalkCore::NUMBER_OF_ANGLES_CALCULATED>
      anglePlotterSubWindows;
   QStatusBar *videoStatusBar;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_MAINWINDOW_H
