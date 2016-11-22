#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QFileInfo>
#include <QPainter>
#include <QRect>
#include <QGridLayout>
#include <QSizePolicy>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <QImage>
#include <QStringList>
#include <ImageViewer.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "cvMat2QtImage.h"
#include "ImageViewer.h"

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow),
   ratWalkTracker(nullptr),
   zoomedRegionWindow(new ImageViewer),
   imageViewerClickedPos(),
   grabbedPointId(-1) {
   ui->setupUi(this);
   setMouseTracking(true);
   ui->ratWalkFrame->setEnabled(false);
}

MainWindow::~MainWindow() {
   delete ui;
   delete ratWalkTracker;
   delete zoomedRegionWindow;
}

void MainWindow::reloadFrame() {
   cv::Mat mat  = ui->checkBoxMostrarEsqueleto->checkState() == Qt::Checked?
                      ratWalkTracker->getFrameWithSkeleton() :
                      ratWalkTracker->getFrameWithRectangle();
   QImage frame = cvMat2QtImage(mat);
   ui->pnlFrame->setImage(frame);
}

void MainWindow::showZoomedRegion(QPoint point, int frameWidth, int frameHeight) {
   if (ratWalkTracker == nullptr) return;
   auto image = ratWalkTracker->getZoomedRegion(point.x(), point.y(), frameWidth, frameHeight);
   QImage frame = cvMat2QtImage(image);
   zoomedRegionWindow->setImage(frame);
   zoomedRegionWindow->resize(image.cols/2, image.rows/2);
   if (!zoomedRegionWindow->isVisible())
      zoomedRegionWindow->show();
}

void MainWindow::on_actionOpen_triggered() {
   int numberOfFrames;
   QString fileName = QFileDialog::getOpenFileName(this, "Open videos",
                            QString(),
                           "RatWalk Files (*.rat);;Other Files (*)");
   if (fileName.isNull()) {
      return;
   }
   on_actionClose_triggered();
   ui->actionClose->setEnabled(true);
   ui->actionGuardar->setEnabled(true);
   ratWalkTracker = new RatWalkTracker(fileName.toStdString().c_str());
   numberOfFrames = ratWalkTracker->getCurrentVideoAnalyzed().NumberOfFrames;
   ui->ratWalkFrame->setEnabled(true);
   ui->horizontalSlider->setMaximum(numberOfFrames);
   reloadFrame();
   QFileInfo fileInfo(fileName);
   QStringList projectName(fileInfo.fileName());
   QTreeWidgetItem *projectRootItem = new QTreeWidgetItem(projectName);
   for (const std::string &videoName : ratWalkTracker->getVideoNames()) {
      QString str = QString::fromStdString(videoName);
      QFileInfo fileInfo(str);
      QStringList fileName(fileInfo.fileName());
      QTreeWidgetItem *videoItem = new QTreeWidgetItem(fileName);
      projectRootItem->addChild(videoItem);
   }
   ui->twProjecto->addTopLevelItem(projectRootItem);
}

void MainWindow::on_btnPrev_clicked() {
   ratWalkTracker->prevFrame();
   reloadFrame();
}

void MainWindow::on_btnNext_clicked() {
   ratWalkTracker->nextFrame();
   ratWalkTracker->guardar();
   reloadFrame();
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
   ratWalkTracker->on_trackbar(value);
   reloadFrame();
}

void MainWindow::on_checkBoxMostrarEsqueleto_stateChanged(int state) {
   reloadFrame();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
   QRect geometry = ui->pnlFrame->geometry();
   geometry.translate(ui->pnlFrame->parentWidget()->mapTo(this, QPoint(0, 0)));
   if (geometry.contains(event->pos())) {
      if (ratWalkTracker == nullptr) return;
      imageViewerClickedPos = ui->pnlFrame->mapFrom(this, event->pos());
      grabbedPointId = ratWalkTracker->getClosestPointID(
                           imageViewerClickedPos.x(),
                           imageViewerClickedPos.y(),
                           ui->pnlFrame->width(),
                           ui->pnlFrame->height(),
                           3.0);
      if (grabbedPointId != -1) {
         ui->pnlFrame->setCursor(Qt::ClosedHandCursor);
      }
   } else {

   }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
   QRect geometry = ui->pnlFrame->geometry();
   geometry.translate(ui->pnlFrame->parentWidget()->mapTo(this, QPoint(0, 0)));
   if (geometry.contains(event->pos())) {
      if (ratWalkTracker == nullptr) return;
      QPoint point = ui->pnlFrame->mapFrom(this, event->pos());
      int frameWidth  = ui->pnlFrame->width();
      int frameHeight = ui->pnlFrame->height();
      if (point == imageViewerClickedPos) {
         int x = point.x(),
             y = point.y();
         ratWalkTracker->addPointOnCurrentFrame(x, y, frameWidth, frameHeight);
         reloadFrame();
         showZoomedRegion(point, frameWidth, frameHeight);
      }
      grabbedPointId = -1;
      ui->pnlFrame->setCursor(Qt::CrossCursor);
   } else {

   }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
   QRect geometry = ui->pnlFrame->geometry();
   geometry.translate(ui->pnlFrame->parentWidget()->mapTo(this, QPoint(0, 0)));
   if (geometry.contains(event->pos())) {
      if (ratWalkTracker == nullptr) return;
      zoomedRegionWindow->show();
      QPoint point = ui->pnlFrame->mapFrom(this, event->pos());
      int frameWidth  = ui->pnlFrame->width();
      int frameHeight = ui->pnlFrame->height();
      if (grabbedPointId != -1) {
         ratWalkTracker->setPointOnCurrentFrame(
                           grabbedPointId,
                           point.x(),
                           point.y(),
                           frameWidth,
                           frameHeight);
         reloadFrame();
      } else {
         int pointToGrabId = ratWalkTracker->getClosestPointID(
                                 point.x(),
                                 point.y(),
                                 frameWidth,
                                 frameHeight,
                                 3.0);
         if (pointToGrabId != -1) {
            ui->pnlFrame->setCursor(Qt::OpenHandCursor);
         } else {
            ui->pnlFrame->setCursor(Qt::CrossCursor);
         }
      }
      showZoomedRegion(point, frameWidth, frameHeight);
   } else {
      grabbedPointId = -1;
      zoomedRegionWindow->hide();
      ui->pnlFrame->setCursor(Qt::ArrowCursor);
   }
}

void MainWindow::on_btnTraerEsqueleto_clicked() {
   ratWalkTracker->traeEsqueleto();
   reloadFrame();
}

void MainWindow::on_twProjecto_doubleClicked(const QModelIndex &index) {
   if (index.parent().isValid()) {
      ratWalkTracker->setCurrentVideo(index.row());
      reloadFrame();
   }
}

void MainWindow::on_actionClose_triggered() {
   if (ratWalkTracker != nullptr) {
      ratWalkTracker->guardar();
      delete ratWalkTracker;
      ratWalkTracker = nullptr;
   }
   ui->twProjecto->clear();
   ui->pnlFrame->setImage(QImage());
   ui->actionClose->setEnabled(false);
   ui->actionGuardar->setEnabled(false);
}

void MainWindow::on_actionGuardar_triggered() {
   ratWalkTracker->guardar();
}
