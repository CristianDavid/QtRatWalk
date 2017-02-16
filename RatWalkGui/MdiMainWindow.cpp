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

#include "RatWalkGui/ImageViewer.h"
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
    QWidget *plottersSubWindows[] = {
        ui->angle1SubWindow->parentWidget(),
        ui->angle2SubWindow->parentWidget(),
        ui->angle3SubWindow->parentWidget(),
        ui->angle4SubWindow->parentWidget(),
        ui->angle5SubWindow->parentWidget()
    };
    for (auto *plotter : plottersSubWindows) {
        plotter->hide();
    }

    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        sub->installEventFilter(this);
        sub->setAttribute(Qt::WA_DeleteOnClose, false);
    }
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
    RatWalkGui::Plotter *plotters[] = {ui->plotterT1, ui->plotterT2, ui->plotterT3, ui->plotterT4, ui->plotterT5};
    RatWalkCore::Video  *videos = ratWalkTracker->getVideos();
    for (RatWalkGui::Plotter *plotter : plotters) {
        plotter->clearPoints();
    }
    for (int i = 0, globalFrame = 0; i < 3; i++) {
        RatWalkCore::Video &video = videos[i];
        for (int j = 0; j < video.NumberOfFrames; j++, globalFrame++) {
            RatWalkCore::Frame &frame = video.FrameProperties[j];
            for (int k = 0; k < frame.NumberOfPointsToTrack; k++) {
                if (k < frame.NumberOfTRegisteredPoints) {
                    RatWalkCore::ControlPoint &point = frame.TrackedPointsInFrame[k];
                    plotters[k]->addPoint(globalFrame, point.ThetaCorrected);
                } else {
                    plotters[k]->addPoint(globalFrame, -1);
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
    QMdiSubWindow *subWindow;
    if ( event->type() == QEvent::Close &&
         (subWindow = qobject_cast<QMdiSubWindow*>(watched)) != nullptr) {
        if (subWindow == ui->videoSubWindow->parentWidget()) {
            ui->actionShow_video->setChecked(false);
        } else if (subWindow == ui->projectSubWindow->parentWidget()) {
            ui->actionShow_projects->setChecked(false);
        }
        return true;
    } else if (watched == ui->pnlFrame && ratWalkTracker != nullptr) {
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

   Video *videos = ratWalkTracker->getVideos();
   int totalFrames = videos[0].NumberOfFrames + videos[1].NumberOfFrames + videos[2].NumberOfFrames;
   for (auto *plotter : {ui->plotterT1, ui->plotterT2, ui->plotterT3, ui->plotterT4, ui->plotterT5}) {
       plotter->setXAxisLength(totalFrames);
       plotter->setYAxisLength(360);
   }
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
      qDebug() << (ui->videoSubWindow->isHidden());
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

void RatWalkGui::MdiMainWindow::on_actionShow_projects_toggled(bool checked) {
    if (checked) {
        ui->projectSubWindow->parentWidget()->show();
        ui->projectSubWindow->show();
    } else {
        ui->projectSubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_video_toggled(bool checked) {
    if (checked) {
        ui->videoSubWindow->parentWidget()->show();
        ui->videoSubWindow->show();
    } else {
        ui->videoSubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_T1_toggled(bool checked) {
    if (checked) {
        ui->angle1SubWindow->parentWidget()->show();
        ui->angle1SubWindow->show();
    } else {
        ui->angle1SubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_T2_toggled(bool checked) {
    if (checked) {
        ui->angle2SubWindow->parentWidget()->show();
        ui->angle2SubWindow->show();
    } else {
        ui->angle2SubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_T3_toggled(bool checked) {
    if (checked) {
        ui->angle3SubWindow->parentWidget()->show();
        ui->angle3SubWindow->show();
    } else {
        ui->angle3SubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_T4_toggled(bool checked) {
    if (checked) {
        ui->angle4SubWindow->parentWidget()->show();
        ui->angle4SubWindow->show();
    } else {
        ui->angle4SubWindow->parentWidget()->hide();
    }
}

void RatWalkGui::MdiMainWindow::on_actionShow_T5_toggled(bool checked) {
    if (checked) {
        ui->angle5SubWindow->parentWidget()->show();
        ui->angle5SubWindow->show();
    } else {
        ui->angle5SubWindow->parentWidget()->hide();
    }
}

/********************************************
 *
 * Callbacks for zooming the plotters,
 * this is terrible hardcoding
 *
 * ******************************************/

void RatWalkGui::MdiMainWindow::on_toolBtnZoomInT1_clicked() {
    QSize defaultSize = ui->plotterT1->parentWidget()->size() - QSize(1, 1);
    ui->plotterT1->resize(ui->plotterT1->size() + defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomOutT1_clicked() {
    QSize defaultSize = ui->plotterT1->parentWidget()->size() - QSize(1, 1);
    ui->plotterT1->resize(ui->plotterT1->size() - defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomFitBestT1_clicked() {
    QSize defaultSize = ui->plotterT1->parentWidget()->size() - QSize(1, 1);
    ui->plotterT1->resize(defaultSize);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomInT2_clicked() {
    QSize defaultSize = ui->plotterT2->parentWidget()->size() - QSize(1, 1);
    ui->plotterT2->resize(ui->plotterT2->size() + defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomOutT2_clicked() {
    QSize defaultSize = ui->plotterT2->parentWidget()->size() - QSize(1, 1);
    ui->plotterT2->resize(ui->plotterT2->size() - defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomFitBestT2_clicked() {
    QSize defaultSize = ui->plotterT2->parentWidget()->size() - QSize(1, 1);
    ui->plotterT2->resize(defaultSize);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomInT3_clicked() {
    QSize defaultSize = ui->plotterT3->parentWidget()->size() - QSize(1, 1);
    ui->plotterT3->resize(ui->plotterT3->size() + defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomOutT3_clicked() {
    QSize defaultSize = ui->plotterT3->parentWidget()->size() - QSize(1, 1);
    ui->plotterT3->resize(ui->plotterT3->size() - defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomFitBestT3_clicked() {
    QSize defaultSize = ui->plotterT3->parentWidget()->size() - QSize(1, 1);
    ui->plotterT3->resize(defaultSize);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomInT4_clicked() {
    QSize defaultSize = ui->plotterT4->parentWidget()->size() - QSize(1, 1);
    ui->plotterT4->resize(ui->plotterT4->size() + defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomOutT4_clicked() {
    QSize defaultSize = ui->plotterT4->parentWidget()->size() - QSize(1, 1);
    ui->plotterT4->resize(ui->plotterT4->size() - defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomFitBestT4_clicked() {
    QSize defaultSize = ui->plotterT4->parentWidget()->size() - QSize(1, 1);
    ui->plotterT4->resize(defaultSize);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomInT5_clicked() {
    QSize defaultSize = ui->plotterT5->parentWidget()->size() - QSize(1, 1);
    ui->plotterT5->resize(ui->plotterT5->size() + defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomOutT5_clicked() {
    QSize defaultSize = ui->plotterT5->parentWidget()->size() - QSize(1, 1);
    ui->plotterT5->resize(ui->plotterT5->size() - defaultSize*0.1);
}

void RatWalkGui::MdiMainWindow::on_toolBtnZoomFitBestT5_clicked() {
    QSize defaultSize = ui->plotterT5->parentWidget()->size() - QSize(1, 1);
    ui->plotterT5->resize(defaultSize);
}
