#include "MdiMainWindow.h"
#include "ui_MdiMainWindow.h"

#include <QMdiSubWindow>
#include <QFileInfo>
#include <QDebug>
#include <QFileDialog>
#include <QEvent>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QMouseEvent>
#include <QAction>
#include <QVariant>
#include <QString>

#include <string>

#include "RatWalkGui/ImageViewer.h"
#include "RatWalkGui/AnglePlotter.h"
#include "RatWalkGui/cvMat2QtImage.h"

namespace RatWalkGui {

MdiMainWindow::MdiMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdiMainWindow),
    ratWalkTracker(nullptr),
    zoomedRegionWindow(new ImageViewer),
    imageViewerClickedPos(),
    grabbedPointId(-1) {
    ui->setupUi(this);

    ui->mdiArea->addSubWindow(zoomedRegionWindow);
    ui->ratWalkFrame->setEnabled(false);
    ui->pnlFrame->installEventFilter(this);

    for (int i = 0; i < anglePlotters.size(); i++) {
        QAction      *showAngleAction;
        AnglePlotter *anglePlotterPtr = new AnglePlotter;
        QVariant      angleActionData;
        std::string   angleName = "T" + std::to_string(i+1);
        QString       nombreAccion  = QString::fromStdString("Mostrar " + angleName);
        anglePlotterPtr->setWindowTitle(angleName.c_str());
        ui->mdiArea->addSubWindow(anglePlotterPtr);
        anglePlotterPtr->parentWidget()->hide();
        showAngleAction = ui->menuShow_angles->addAction(nombreAccion);
        angleActionData.setValue<QWidget*>(anglePlotterPtr);
        showAngleAction->setData(angleActionData);
        QObject::connect(showAngleAction, &QAction::triggered,
                         this,            &MdiMainWindow::onActionsShowSubWindowTriggered);
        anglePlotters[i] = anglePlotterPtr;
    }

    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        sub->setAttribute(Qt::WA_DeleteOnClose, false);
    }

    QVariant projectSubWindowData;
    projectSubWindowData.setValue<QWidget*>(ui->projectSubWindow);
    ui->actionShow_projects->setData(projectSubWindowData);
    QObject::connect(ui->actionShow_projects, &QAction::triggered,
                     this, &MdiMainWindow::onActionsShowSubWindowTriggered);

    QVariant videoSubWindowData;
    videoSubWindowData.setValue<QWidget*>(ui->videoSubWindow);
    ui->actionShow_video->setData(videoSubWindowData);
    QObject::connect(ui->actionShow_video, &QAction::triggered,
                     this, &MdiMainWindow::onActionsShowSubWindowTriggered);

}

MdiMainWindow::~MdiMainWindow() {
    delete ui;
    delete ratWalkTracker;
    delete zoomedRegionWindow;
}

void MdiMainWindow::reloadFrame() {
    cv::Mat mat  = ui->checkBoxMostrarEsqueleto->checkState() == Qt::Checked?
                       ratWalkTracker->getFrameWithSkeleton() :
                       ratWalkTracker->getFrameWithRectangle();
    QImage frame = cvMat2QtImage(mat);
    ui->pnlFrame->setImage(frame);
    RatWalkCore::Video  *videos = ratWalkTracker->getVideos();
    for (AnglePlotter *anglePlotter : anglePlotters) {
        anglePlotter->getPlotter()->clearPoints();
    }
    for (int i = 0, globalFrame = 0; i < 3; i++) {
        RatWalkCore::Video &video = videos[i];
        for (int j = 0; j < video.NumberOfFrames; j++, globalFrame++) {
            RatWalkCore::Frame &frame = video.FrameProperties[j];
            for (int k = 0; k < frame.NumberOfPointsToTrack; k++) {
                if (k < frame.NumberOfTRegisteredPoints) {
                    RatWalkCore::ControlPoint &point = frame.TrackedPointsInFrame[k];
                    anglePlotters[k]->getPlotter()->addPoint(globalFrame, point.ThetaCorrected);
                } else {
                    anglePlotters[k]->getPlotter()->addPoint(globalFrame, -1);
                }
            }
        }
    }
}

void MdiMainWindow::showZoomedRegion(QPoint point, int frameWidth, int frameHeight) {
    auto image = ratWalkTracker->getZoomedRegion(point.x(), point.y(), frameWidth, frameHeight);
    QImage frame = cvMat2QtImage(image);
    zoomedRegionWindow->setImage(frame);
    zoomedRegionWindow->resize(image.cols/2, image.rows/2);
    if (!zoomedRegionWindow->isVisible())
       zoomedRegionWindow->show();

}

void MdiMainWindow::mousePressEventOnPnlFrame(QMouseEvent *event) {
   QRect geometry(QPoint(0, 0), ui->pnlFrame->size());
   if (event->button() == Qt::LeftButton && geometry.contains(event->pos())) {
      imageViewerClickedPos = event->pos();
      grabbedPointId = ratWalkTracker->getClosestPointID(
                            imageViewerClickedPos.x(),
                            imageViewerClickedPos.y(),
                            ui->pnlFrame->width(),
                            ui->pnlFrame->height(),
                            3.0);
      if (grabbedPointId != -1) {
         ui->pnlFrame->setCursor(Qt::ClosedHandCursor);
      }
   }
}

void MdiMainWindow::mouseReleaseEventOnPnlFrame(QMouseEvent *event) {
    QRect geometry(QPoint(0, 0), ui->pnlFrame->size());
    if (geometry.contains(event->pos())) {
       int pointToDeleteId = pointToGrabId(event->pos(), 3.0);
       if (event->button() == Qt::RightButton && pointToDeleteId != -1) {
          QMenu menu("Puntos", this);
          ui->actionDelete_point->setData(pointToDeleteId);
          menu.addAction(ui->actionDelete_point);
          menu.exec(event->globalPos());
       } else if (event->button() == Qt::LeftButton) {
          QPoint point    = event->pos();
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
       }
    }
}

void MdiMainWindow::mouseMoveEventOnPnlFrame(QMouseEvent *event) {
    QRect geometry(QPoint(0, 0), ui->pnlFrame->size());
    if (geometry.contains(event->pos())) {
       zoomedRegionWindow->show();
       QPoint point       = event->pos();
       int    frameWidth  = ui->pnlFrame->width();
       int    frameHeight = ui->pnlFrame->height();
       if (grabbedPointId != -1) {
          ratWalkTracker->setPointOnCurrentFrame(
                            grabbedPointId,
                            point.x(),
                            point.y(),
                            frameWidth,
                            frameHeight);
          reloadFrame();
       } else {
          if (pointToGrabId(point, 3.0) != -1) {
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

bool MdiMainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->pnlFrame && ratWalkTracker != nullptr) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                mousePressEventOnPnlFrame((QMouseEvent*)event);
                break;
            case QEvent::MouseButtonRelease:
                mouseReleaseEventOnPnlFrame((QMouseEvent*)event);
                break;
            case QEvent::MouseMove:
                mouseMoveEventOnPnlFrame((QMouseEvent*)event);
                break;
            default:
                ;
                break;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

} // namespace RatWalkGui

void RatWalkGui::MdiMainWindow::onActionsShowSubWindowTriggered() {
    QAction *senderAction    = static_cast<QAction*>(sender());
    QWidget *subWindowWidget = senderAction->data().value<QWidget*>();
    subWindowWidget->parentWidget()->show();
    subWindowWidget->show();
}

void RatWalkGui::MdiMainWindow::on_actionOpen_triggered() {
   using RatWalkCore::Tracker;
   using RatWalkCore::Video;
   int numberOfFrames;
   QString fileName = QFileDialog::getOpenFileName(this, "Open videos",
                             QString(),
                            "RatWalk Files (*.rat);;Other Files (*)");
   if (fileName.isNull()) {
      return;
   }
   on_actionClose_triggered();
   ui->actionClose->setEnabled(true);
   ui->actionSave->setEnabled(true);
   ratWalkTracker = new Tracker(fileName.toStdString().c_str());
   numberOfFrames = ratWalkTracker->getCurrentVideoAnalyzed().NumberOfFrames;
   ui->ratWalkFrame->setEnabled(true);
   ui->horizontalSlider->setMaximum(numberOfFrames-1);
   ui->spinBoxCambiarFrame->setMinimum(0);
   ui->spinBoxCambiarFrame->setMaximum(numberOfFrames-1);

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

   Video *videos = ratWalkTracker->getVideos();

   std::vector<int> framesPerVideo;
   for (int i = 0; i < 3; i++) {
        framesPerVideo.push_back(videos[i].NumberOfFrames);
   }
   for (AnglePlotter *anglePlotter : anglePlotters) {
       anglePlotter->setFramesPerVideo(framesPerVideo);
   }
   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_actionSave_triggered() {
   ratWalkTracker->guardar();
}

void RatWalkGui::MdiMainWindow::on_actionClose_triggered() {
   if (ratWalkTracker != nullptr) {
      ratWalkTracker->guardar();
      delete ratWalkTracker;
      ratWalkTracker = nullptr;
   }
   ui->twProjecto->clear();
   ui->pnlFrame->setImage(QImage());
   ui->actionClose->setEnabled(false);
   ui->actionSave->setEnabled(false);
}

void RatWalkGui::MdiMainWindow::on_btnNext_clicked() {
   bool signalsEnabled;
   ratWalkTracker->nextFrame();
   ratWalkTracker->guardar();
   int  currentFrame = ratWalkTracker->getCurrentVideoAnalyzed().CurrentFrame;

   signalsEnabled = ui->horizontalSlider->blockSignals(true);
   ui->horizontalSlider->setValue(currentFrame);
   ui->horizontalSlider->blockSignals(signalsEnabled);

   signalsEnabled = ui->spinBoxCambiarFrame->blockSignals(true);
   ui->spinBoxCambiarFrame->setValue(currentFrame);
   ui->spinBoxCambiarFrame->blockSignals(signalsEnabled);

   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_btnPrev_clicked() {
   bool signalsEnabled;
   ratWalkTracker->prevFrame();
   int  currentFrame = ratWalkTracker->getCurrentVideoAnalyzed().CurrentFrame;

   signalsEnabled = ui->horizontalSlider->blockSignals(true);
   ui->horizontalSlider->setValue(currentFrame);
   ui->horizontalSlider->blockSignals(signalsEnabled);

   signalsEnabled = ui->spinBoxCambiarFrame->blockSignals(true);
   ui->spinBoxCambiarFrame->setValue(currentFrame);
   ui->spinBoxCambiarFrame->blockSignals(signalsEnabled);

   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_btnTraerEsqueleto_clicked() {
   ratWalkTracker->traeEsqueleto();
   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_checkBoxMostrarEsqueleto_stateChanged(int state) {
    reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_horizontalSlider_valueChanged(int value) {
   ratWalkTracker->on_trackbar(value);
   bool signalsEnabled = ui->spinBoxCambiarFrame->blockSignals(true);
   ui->spinBoxCambiarFrame->setValue(value);
   ui->spinBoxCambiarFrame->blockSignals(signalsEnabled);
   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_spinBoxCambiarFrame_valueChanged(int value) {
   bool signalsEnabled;
   ratWalkTracker->on_trackbar(value);
   signalsEnabled = ui->horizontalSlider->blockSignals(true);
   ui->horizontalSlider->setValue(value);
   ui->horizontalSlider->blockSignals(signalsEnabled);
   reloadFrame();
}

void RatWalkGui::MdiMainWindow::on_twProjecto_doubleClicked(const QModelIndex &index) {
   if (index.parent().isValid()) {
      ratWalkTracker->setCurrentVideo(index.row());
      bool signalsEnabled = ui->spinBoxCambiarFrame->blockSignals(true);
      ui->spinBoxCambiarFrame->setValue(ratWalkTracker->getCurrentVideoAnalyzed().CurrentFrame);
      ui->spinBoxCambiarFrame->blockSignals(signalsEnabled);
      reloadFrame();
   }
}

int RatWalkGui::MdiMainWindow::pointToGrabId(QPoint pos, double radius) {
   if (ratWalkTracker == nullptr) return -1;
   int w = ui->pnlFrame->width(),
       h = ui->pnlFrame->height();
   return ratWalkTracker->getClosestPointID(pos.x(), pos.y(), w, h, radius);
}

void RatWalkGui::MdiMainWindow::on_actionDelete_point_triggered() {
    ratWalkTracker->deletePointOnCurrentFrame(ui->actionDelete_point->data().toInt());
    reloadFrame();
}

