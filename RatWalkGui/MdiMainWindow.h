#ifndef RATWALKGUI_MDIMAINWINDOW_H
#define RATWALKGUI_MDIMAINWINDOW_H

#include <QMainWindow>

#include "RatWalkCore/Tracker.h"
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

   void on_actionShow_projects_toggled(bool checked);

   void on_actionShow_video_toggled(bool checked);

   void on_actionShow_T1_toggled(bool checked);
   void on_actionShow_T2_toggled(bool checked);
   void on_actionShow_T3_toggled(bool checked);
   void on_actionShow_T4_toggled(bool checked);
   void on_actionShow_T5_toggled(bool checked);

   //this slots are a terrible case of hardcoding
   void on_toolBtnZoomInT1_clicked();
   void on_toolBtnZoomOutT1_clicked();
   void on_toolBtnZoomFitBestT1_clicked();
   void on_toolBtnZoomInT2_clicked();
   void on_toolBtnZoomOutT2_clicked();
   void on_toolBtnZoomFitBestT2_clicked();
   void on_toolBtnZoomInT3_clicked();
   void on_toolBtnZoomOutT3_clicked();
   void on_toolBtnZoomFitBestT3_clicked();
   void on_toolBtnZoomInT4_clicked();
   void on_toolBtnZoomOutT4_clicked();
   void on_toolBtnZoomFitBestT4_clicked();
   void on_toolBtnZoomInT5_clicked();
   void on_toolBtnZoomOutT5_clicked();
   void on_toolBtnZoomFitBestT5_clicked();

private:
   Ui::MdiMainWindow *ui;
   RatWalkCore::Tracker *ratWalkTracker;
   ImageViewer *zoomedRegionWindow;
   QPoint imageViewerClickedPos;
   int grabbedPointId;
};


} // namespace RatWalkGui
#endif // RATWALKGUI_MDIMAINWINDOW_H
