#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include "RatWalkTracker.h"
#include "ImageViewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
   Q_OBJECT
public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();
   void reloadFrame();
   void showZoomedRegion(QPoint point, int frameWidth, int frameHeight);
private slots:
   void on_actionOpen_triggered();
   void on_btnPrev_clicked();
   void on_btnNext_clicked();
   void on_horizontalSlider_valueChanged(int value);
   void on_checkBoxMostrarEsqueleto_stateChanged(int state);
   void on_btnTraerEsqueleto_clicked();
   void on_twProjecto_doubleClicked(const QModelIndex &index);
   void on_actionClose_triggered();

   void on_pushButton_clicked();

   void on_actionGuardar_triggered();

protected:
   void mousePressEvent(  QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(   QMouseEvent *event);
private:
   Ui::MainWindow *ui;
   RatWalkTracker *ratWalkTracker;
   ImageViewer    *zoomedRegionWindow;
   QPoint          imageViewerClickedPos;
   int             grabbedPointId;
};

#endif // MAINWINDOW_H
