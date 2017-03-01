#ifndef RATWALKGUI_MDIMAINWINDOW_H
#define RATWALKGUI_MDIMAINWINDOW_H

#include <array>
#include <memory>
#include <vector>

#include <QMainWindow>
#include <QStatusBar>

#include "RatWalkCore/Tracker.h"
#include "RatWalkGui/AnglePlotter.h"
#include "RatWalkGui/ImageViewer.h"

namespace RatWalkGui {

namespace Ui {
class MdiMainWindow;
}

class MdiMainWindow : public QMainWindow {
   Q_OBJECT

public:
   explicit MdiMainWindow(QWidget *parent = 0);
   ~MdiMainWindow();

    void reloadFrame();

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

private:
   typedef std::shared_ptr<RatWalkCore::Tracker> ProjectPtr;
   typedef std::vector<ProjectPtr> ProjectVector;

   void onFrameNumberChanged();
   void updateStepInfo();
   void loadSteps();
   void setCurrentProject(int projectIdx);
   ProjectPtr getCurrentProject();

   Ui::MdiMainWindow *ui;
   ProjectVector projects;
   int currentProjectIdx;
   ImageViewer *zoomedRegionWindow;
   QPoint imageViewerClickedPos;
   int grabbedPointId;
   int stepBegin = -1;
   std::array<RatWalkGui::AnglePlotter*, 5> anglePlotters;
   QStatusBar *videoStatusBar;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_MDIMAINWINDOW_H
