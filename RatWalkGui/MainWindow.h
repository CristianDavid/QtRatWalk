#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QObject>
#include "RatWalkCore/Tracker.h"
#include "RatWalkGui/ImageViewer.h"

namespace Ui {
class MainWindow;
}

namespace RatWalkGui {

class MainWindow : public QMainWindow {
   Q_OBJECT
public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();
   void reloadFrame();
   void showZoomedRegion(QPoint point, int frameWidth, int frameHeight);
private slots:
   void on_actionBorrarPunto_triggered();
   void on_actionClose_triggered();
   void on_actionGuardar_triggered();
   void on_actionOpen_triggered();
   void on_btnNext_clicked();
   void on_btnPrev_clicked();
   void on_btnTraerEsqueleto_clicked();
   void on_checkBoxMostrarEsqueleto_stateChanged(int state);
   void on_horizontalSlider_valueChanged(int value);
   void on_spinBoxCambiarFrame_valueChanged(int value);
   void on_twProjecto_doubleClicked(const QModelIndex &index);
protected:
   void mousePressEvent(  QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(   QMouseEvent *event);
private:
   int pointToGrabId(QPoint pos, double radius);
   Ui::MainWindow *ui;
   RatWalkCore::Tracker *ratWalkTracker;
   ImageViewer *zoomedRegionWindow;
   QPoint imageViewerClickedPos;
   int grabbedPointId;
};

} // namespace RatWalkGui

#endif // MAINWINDOW_H
