#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <climits>
#include <atomic>
#include <string>
#include <thread>
#include <QApplication>
#include <QCoreApplication>
#include <QMdiSubWindow>
#include <QFileInfo>
#include <QDebug>
#include <QDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QEvent>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QMouseEvent>
#include <QStatusBar>
#include <QSizePolicy>
#include <QAction>
#include <QVariant>
#include <QString>

#include "RatWalkGui/ImageViewer.h"
#include "RatWalkGui/AnglePlotter.h"
#include "RatWalkGui/cvMat2QtImage.h"
#include "RatWalkGui/ExportAnglesDialog.h"
#include "RatWalkGui/ShowImageDialog.h"
#include "xlsxdocument.h"

namespace RatWalkGui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentProjectIdx(-1),
    zoomedRegionWindow(new ImageViewer),
    imageViewerClickedPos(),
    grabbedPointId(-1) {
    ui->setupUi(this);
    videoStatusBar = new QStatusBar;
    videoStatusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
    ui->statusBarLayout->addWidget(videoStatusBar);
    zoomedRegionWindow->setWindowTitle("zoom");
    ui->mdiArea->addSubWindow(zoomedRegionWindow);
    ui->ratWalkFrame->setEnabled(false);
    ui->pnlFrame->installEventFilter(this);

    anglePlotters.push_back(AnglePlotterArray());
    for (int i = 0; i < (int)getAnglePlotters().size(); i++) {
        QAction      *showAngleAction;
        AnglePlotter *anglePlotterPtr = new AnglePlotter;
        QVariant      angleActionData;
        std::string   angleName = "T" + std::to_string(i+1); //! todo change title according to current project
        QString       nombreAccion  = QString::fromStdString("Mostrar " + angleName);
        anglePlotterPtr->setWindowTitle(angleName.c_str());
        anglePlotterSubWindows[i].setWidget(anglePlotterPtr);
        ui->mdiArea->addSubWindow(&anglePlotterSubWindows[i]);
        anglePlotterPtr->parentWidget()->hide();
        showAngleAction = ui->menuShow_angles->addAction(nombreAccion);
        angleActionData.setValue<QMdiSubWindow*>(&anglePlotterSubWindows[i]);
        showAngleAction->setData(angleActionData);
        QObject::connect(showAngleAction, &QAction::triggered,
                         this,            &MainWindow::onActionsShowSubWindowTriggered);
        getAnglePlotters()[i] = anglePlotterPtr;
    }

    for (QMdiSubWindow *sub : ui->mdiArea->subWindowList()) {
        sub->setAttribute(Qt::WA_DeleteOnClose, false);
    }

    QVariant projectSubWindowData;
    projectSubWindowData.setValue<QWidget*>(ui->projectSubWindow->parentWidget());
    ui->actionShow_projects->setData(projectSubWindowData);
    QObject::connect(ui->actionShow_projects, &QAction::triggered,
                     this, &MainWindow::onActionsShowSubWindowTriggered);

    QVariant videoSubWindowData;
    videoSubWindowData.setValue<QWidget*>(ui->videoSubWindow->parentWidget());
    ui->actionShow_video->setData(videoSubWindowData);
    QObject::connect(ui->actionShow_video, &QAction::triggered,
                     this, &MainWindow::onActionsShowSubWindowTriggered);
}

MainWindow::~MainWindow() {
    delete ui;
    delete zoomedRegionWindow;
}

void MainWindow::reloadFrame() {
    cv::Mat mat  = ui->checkBoxMostrarEsqueleto->checkState() == Qt::Checked?
                       getCurrentProject()->getFrameWithSkeleton() :
                       getCurrentProject()->getFrameWithRectangle();
    QImage frame = cvMat2QtImage(mat);
    ui->pnlFrame->setImage(frame);
    std::vector<RatWalkCore::Video> &videos = getCurrentProject()->getVideos();
    for (AnglePlotter *anglePlotter : getAnglePlotters()) {
        anglePlotter->getPlotter()->clearPoints();
    }
    for (int i = 0, globalFrame = 0; i < 3; i++) {
        RatWalkCore::Video &video = videos[i];
        for (int j = 0; j < video.NumberOfFrames; j++, globalFrame++) {
            RatWalkCore::Frame &frame = video.FrameProperties[j];
            for (int k = 0; k < frame.NumberOfPointsToTrack; k++) {
                if (k < frame.NumberOfTRegisteredPoints) {
                    RatWalkCore::ControlPoint &point = frame.TrackedPointsInFrame[k];
                    getAnglePlotters()[k]->getPlotter()->addPoint(globalFrame, point.ThetaCorrected);
                } else {
                    getAnglePlotters()[k]->getPlotter()->addPoint(globalFrame, -1);
                }
            }
        }
    }
}

void MainWindow::showZoomedRegion(QPoint point, int frameWidth, int frameHeight) {
    auto image = getCurrentProject()->getZoomedRegion(
             point.x(), point.y(),
             frameWidth, frameHeight
         );
    QImage frame = cvMat2QtImage(image);
    zoomedRegionWindow->setImage(frame);
}

void MainWindow::mousePressEventOnPnlFrame(QMouseEvent *event) {
   QRect geometry(QPoint(0, 0), ui->pnlFrame->size());
   if (event->button() == Qt::LeftButton && geometry.contains(event->pos())) {
      imageViewerClickedPos = event->pos();
      grabbedPointId = getCurrentProject()->getClosestPointID(
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

void MainWindow::mouseReleaseEventOnPnlFrame(QMouseEvent *event) {
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
             getCurrentProject()->addPointOnCurrentFrame(x, y, frameWidth, frameHeight);
             reloadFrame();
             showZoomedRegion(point, frameWidth, frameHeight);
          }
          grabbedPointId = -1;
          ui->pnlFrame->setCursor(Qt::CrossCursor);
       }
    }
}

void MainWindow::mouseMoveEventOnPnlFrame(QMouseEvent *event) {
    QRect geometry(QPoint(0, 0), ui->pnlFrame->size());
    if (geometry.contains(event->pos())) {
       zoomedRegionWindow->show();
       QPoint point       = event->pos();
       int    frameWidth  = ui->pnlFrame->width();
       int    frameHeight = ui->pnlFrame->height();
       if (grabbedPointId != -1) {
          getCurrentProject()->setPointOnCurrentFrame(
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

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->pnlFrame && getCurrentProject() != nullptr) {
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

void MainWindow::onActionsShowSubWindowTriggered() {
    QAction *senderAction    = static_cast<QAction*>(sender());
    QMdiSubWindow *subWindow = senderAction->data().value<QMdiSubWindow*>();
    subWindow->show();
    subWindow->widget()->show();
}

void MainWindow::on_actionOpen_triggered() {
   using RatWalkCore::Project;
   using RatWalkCore::Video;
   QString fileName = QFileDialog::getOpenFileName(
      this,
      "Open videos",
      "",
      "RatWalk Files (*.rat);;Other Files (*)"
   );
   if (fileName.isNull()) return;

   std::atomic<RatWalkCore::Project *> newProjectPtr(nullptr);
   std::thread openProject([&newProjectPtr, &fileName] {
      newProjectPtr = new Project(fileName.toStdString().c_str());
   });
   QApplication::setOverrideCursor(Qt::BusyCursor);
   setEnabled(false);
   ui->statusbar->showMessage("Abriendo proyecto");
   while (newProjectPtr == nullptr) {
      QCoreApplication::processEvents();
   }
   openProject.join();
   ui->statusbar->showMessage("Proyecto abierto", 10000);
   QApplication::restoreOverrideCursor();
   setEnabled(true);
   projects.push_back(ProjectPtr(newProjectPtr.load()));
   setCurrentProject(projects.size() - 1);

   QFileInfo fileInfo(fileName);
   QStringList projectName(fileInfo.fileName());
   QTreeWidgetItem *projectRootItem = new QTreeWidgetItem(projectName);
   for (const std::string &videoName : getCurrentProject()->getVideoFilenames()) {
      QString str = QString::fromStdString(videoName);
      QFileInfo fileInfo(str);
      QStringList fileName(fileInfo.fileName());
      QTreeWidgetItem *videoItem = new QTreeWidgetItem(fileName);
      projectRootItem->addChild(videoItem);
   }
   ui->twProjecto->addTopLevelItem(projectRootItem);

   std::vector<Video> &videos = getCurrentProject()->getVideos();

   std::vector<int> framesPerVideo;
   for (int i = 0; i < getCurrentProject()->getSize(); i++) {
        framesPerVideo.push_back(videos[i].NumberOfFrames);
   }
   anglePlotters.push_back(AnglePlotterArray());
   AnglePlotterArray &newAnglePlotters = getAnglePlotters();
   for (int i = 0; i < (int)anglePlotterSubWindows.size(); i++) {
      QString title = "T" + QString::number(i+1) + " [" +
                      projectName.front() + "]";
      newAnglePlotters[i] = new AnglePlotter;
      newAnglePlotters[i]->setWindowTitle(title);
      anglePlotterSubWindows[i].setWidget(newAnglePlotters[i]);
   }
   for (AnglePlotter *anglePlotter : getAnglePlotters()) {
       anglePlotter->setFramesPerVideo(framesPerVideo);
   }
   loadSteps();
   reloadFrame();
}

void MainWindow::on_actionSave_triggered() {
   getCurrentProject()->save();
}

void MainWindow::on_actionClose_triggered() {
   getCurrentProject()->save();
   projects.erase(projects.begin() + currentProjectIdx);
   ui->twProjecto->takeTopLevelItem(currentProjectIdx);
   setCurrentProject(projects.size()-1);
   for (int i = 0; i < (int)anglePlotterSubWindows.size(); i++) {
      anglePlotterSubWindows[i].setWidget(getAnglePlotters()[i]);
   }
   anglePlotters.pop_back();
}

void MainWindow::on_btnNext_clicked() {
   getCurrentProject()->nextFrame();
   getCurrentProject()->save();
   onFrameNumberChanged();
   reloadFrame();
}

void MainWindow::on_btnPrev_clicked() {
   getCurrentProject()->prevFrame();
   onFrameNumberChanged();
   reloadFrame();
}

void MainWindow::on_btnTraerEsqueleto_clicked() {
   getCurrentProject()->bringPreviousSkeleton();
   reloadFrame();
}

void MainWindow::on_checkBoxMostrarEsqueleto_stateChanged(int state) {
    (void) state;
    reloadFrame();
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
   getCurrentProject()->setFrame(value);
   onFrameNumberChanged();
   reloadFrame();
}

void MainWindow::on_spinBoxCambiarFrame_valueChanged(int value) {
   getCurrentProject()->setFrame(value);
   onFrameNumberChanged();
   reloadFrame();
}

void MainWindow::on_twProjecto_doubleClicked(const QModelIndex &index) {
   int currentProject;
   int currentVideo;
   if (index.parent().isValid()) {
      currentProject = index.parent().row();
      currentVideo   = index.row();
   } else {
      currentProject = index.row();
      currentVideo   = 0;
   }
   stepBegin = -1;
   setCurrentProject(currentProject);
   for (int i = 0; i < (int)anglePlotterSubWindows.size(); i++) {
      AnglePlotter *anglePlotter = getAnglePlotters()[i];
      QMdiSubWindow &subWindow   = anglePlotterSubWindows[i];
      if (subWindow.widget() != anglePlotter) {
         subWindow.setWidget(anglePlotter);
      }
   }
   getCurrentProject()->setCurrentVideo(currentVideo);
   onFrameNumberChanged();
   reloadFrame();
}

int MainWindow::pointToGrabId(QPoint pos, double radius) {
   ProjectPtr currentProject = getCurrentProject();
   if (currentProject == nullptr) return -1;
   int w = ui->pnlFrame->width(),
       h = ui->pnlFrame->height();
   return currentProject->getClosestPointID(pos.x(), pos.y(), w, h, radius);
}

void MainWindow::on_actionDelete_point_triggered() {
    getCurrentProject()->deletePointOnCurrentFrame(ui->actionDelete_point->data().toInt());
    reloadFrame();
}

void MainWindow::onFrameNumberChanged() {
    bool signalsEnabled;
    int currentFrame = getCurrentProject()->getCurrentVideoAnalyzed().CurrentFrame;
    signalsEnabled = ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setValue(currentFrame);
    ui->horizontalSlider->blockSignals(signalsEnabled);

    signalsEnabled = ui->spinBoxCambiarFrame->blockSignals(true);
    ui->spinBoxCambiarFrame->setValue(currentFrame);
    ui->spinBoxCambiarFrame->blockSignals(signalsEnabled);
    updateStepInfo();
}
void MainWindow::updateStepInfo() {
    int currentFrame = getCurrentProject()->getCurrentVideoAnalyzed().CurrentFrame;
    RatWalkCore::StepRegister &stepRegister = getCurrentProject()->getCurrentStepRegister();
    if (stepBegin == -1) {
        ui->btnDiscardStep->setEnabled(false);
        ui->btnDiscardStep->setVisible(false);
        if (stepRegister.posNotOverlapping(currentFrame)) {
            videoStatusBar->clearMessage();
            ui->btnStartStep->setVisible(true);
            ui->btnStartStep->setEnabled(true);
            ui->btnFinishStep->setVisible(true);
            ui->btnFinishStep->setEnabled(false);
            ui->btnEreaseStep->setVisible(false);
            ui->btnEreaseStep->setEnabled(false);
        } else {
            auto step = stepRegister.getSurroundingStep(currentFrame);
            videoStatusBar->showMessage(
                "Inicio de paso: " + QString::number(step.first) +
                ", Fin de paso: "  + QString::number(step.second)
            );
            ui->btnStartStep->setVisible(false);
            ui->btnStartStep->setEnabled(false);
            ui->btnEreaseStep->setVisible(true);
            ui->btnEreaseStep->setEnabled(true);
            ui->btnFinishStep->setVisible(false);
            ui->btnFinishStep->setEnabled(false);
        }
    } else {
        ui->btnDiscardStep->setEnabled(true);
        ui->btnDiscardStep->setVisible(true);
        ui->btnEreaseStep->setVisible(false);
        ui->btnEreaseStep->setEnabled(false);
        videoStatusBar->showMessage(
            "Paso iniciado en frame: " + QString::number(stepBegin)
        );
        ui->btnStartStep->setVisible(false);
        ui->btnStartStep->setEnabled(false);
        ui->btnFinishStep->setVisible(true);
        if (stepBegin < currentFrame) {
            if (stepRegister.stepNotOverlapping(stepBegin, currentFrame)) {
                ui->btnFinishStep->setEnabled(true);
            } else {
                auto overlap = stepRegister.getSurroundingStep(stepBegin, currentFrame);
                ui->btnFinishStep->setEnabled(false);
                videoStatusBar->showMessage(
                    videoStatusBar->currentMessage() +
                    ", se traslapa con [" + QString::number(overlap.first) +
                    "," + QString::number(overlap.second) + "]"
                );
            }
        } else {
            ui->btnFinishStep->setEnabled(false);
        }
    }
}

void MainWindow::loadSteps() {
    using RatWalkCore::StepRegister;
    std::vector<StepRegister> &registers =
          getCurrentProject()->getStepRegisters();
    for (int i = 0; i < getCurrentProject()->getSize(); i++) {
        std::vector<StepRegister::Step> steps = registers[i].getSteps();
        for (auto step : steps) {
            for (AnglePlotter *plotter : getAnglePlotters()) {
                plotter->addStep(i, step.first, step.second);
            }
        }
    }
}

void MainWindow::setCurrentProject(int projectIdx) {
    currentProjectIdx = projectIdx;
    if (currentProjectIdx == -1) {
        ui->pnlFrame->setImage(QImage());
        zoomedRegionWindow->setImage(QImage());
        ui->ratWalkFrame->setEnabled(false);
        ui->actionClose->setEnabled(false);
        ui->actionSave->setEnabled(false);
    } else {
        int numberOfFrames = getCurrentProject()->getCurrentVideoAnalyzed().NumberOfFrames;
        ui->ratWalkFrame->setEnabled(true);
        ui->horizontalSlider->setMaximum(numberOfFrames-1);
        ui->spinBoxCambiarFrame->setMinimum(0);
        ui->spinBoxCambiarFrame->setMaximum(numberOfFrames-1);
        ui->actionClose->setEnabled(true);
        ui->actionSave->setEnabled(true);
        onFrameNumberChanged();
    }
}

MainWindow::ProjectPtr MainWindow::getCurrentProject() {
    if (currentProjectIdx != -1) {
        return projects[currentProjectIdx];
    } else {
        return ProjectPtr();
    }
}

MainWindow::AnglePlotterArray &MainWindow::getAnglePlotters() {
   return anglePlotters[currentProjectIdx+1];
}

void MainWindow::on_btnStartStep_clicked() {
    stepBegin = getCurrentProject()->getCurrentVideoAnalyzed().CurrentFrame;
    updateStepInfo();
}

void MainWindow::on_btnFinishStep_clicked() {
   RatWalkCore::StepRegister &stepRegister =
         getCurrentProject()->getCurrentStepRegister();
   int stepEnd = getCurrentProject()->getCurrentVideoAnalyzed().CurrentFrame;
   stepRegister.addStep(stepBegin, stepEnd);
   int videoIdx = getCurrentProject()->getCurrentVideoIndex();
   for (AnglePlotter *plotter : getAnglePlotters()) {
      plotter->addStep(videoIdx, stepBegin, stepEnd);
   }
   stepBegin = -1;
   updateStepInfo();
}

void MainWindow::on_btnDiscardStep_clicked() {
   stepBegin = -1;
   updateStepInfo();
}

void MainWindow::on_btnEreaseStep_clicked() {
   using RatWalkCore::StepRegister;
   StepRegister &stepRegister = getCurrentProject()->getCurrentStepRegister();
   int pos = getCurrentProject()->getCurrentVideoAnalyzed().CurrentFrame;
   stepRegister.ereaseSurroundingStep(pos);
   for (AnglePlotter *plotter : getAnglePlotters()) {
      plotter->clearSteps();
   }
   loadSteps();
   updateStepInfo();
}

void MainWindow::on_actionExport_angles_triggered() {
   using RatWalkCore::Project;
   std::vector<const char*> openProjectsNames;
   for (ProjectPtr projectPtr : projects) {
      openProjectsNames.push_back(projectPtr->getProjectName());
   }
   ExportAnglesDialog *dialog = new ExportAnglesDialog(
         openProjectsNames,
         this
   );
   if (dialog->exec() == QDialog::Accepted) {
      QXlsx::Document xlsx;
      auto exportOrder = dialog->getExportOrder();
      xlsx.deleteSheet("Sheet1");

      auto ordersAndOrientations = {
         std::make_pair(exportOrder.second, "der"),
         std::make_pair(exportOrder.first, "izq")
      };

      int sheetNumber = 0;
      for (auto &orderAndOrientation : ordersAndOrientations) {
         std::vector<int> order  = orderAndOrientation.first;
         QString orientation(orderAndOrientation.second);
         for (int i = 0; i < (int)order.size(); i++) {
            int projectNumber = order[i];
            ProjectPtr project = projects[projectNumber];
            auto &stepRegisters = project->getStepRegisters();
            auto &videos = project->getVideos();
            for (int angle = 0; angle < RatWalkCore::NUMBER_OF_ANGLES_CALCULATED; angle++) {
               QString sheetName =
                     "T" + QString::number(angle+1) +
                     " v" + QString::number(i+1) + orientation;
               xlsx.insertSheet(sheetNumber, sheetName);
               xlsx.selectSheet(sheetName);
               for (int k = 0, column = 1; k < project->getSize(); k++) {
                  RatWalkCore::StepRegister &stepRegister = stepRegisters[k];
                  RatWalkCore::Video &video = videos[k];
                  auto stepVector = stepRegister.getSteps();
                  for (int stepNumber = 0; stepNumber < (int)stepVector.size(); stepNumber++, column++) {
                     auto step = stepVector[stepNumber];
                     for (int row = 1, frameNumber = step.first; frameNumber <= step.second; frameNumber++, row++) {
                        xlsx.write(row, column,
                           video.FrameProperties[frameNumber].TrackedPointsInFrame[angle].ThetaCorrected
                        );
                     }
                  }
               }
               sheetNumber++;
            }
         }
      }

      QString saveFilename = QFileDialog::getSaveFileName(
         this,
         "Exportar ángulos",
          QString(),
          "Excel (*.xlsx)");
      if (!saveFilename.isNull()) {
         if (!saveFilename.endsWith(".xlsx")) {
            saveFilename += ".xlsx";
         }
         if(xlsx.saveAs(saveFilename)) {
            ui->statusbar->showMessage("Ángulos exportados a " + saveFilename, 10000);
         } else {
            ui->statusbar->showMessage("No se pudo guardar " + saveFilename, 10000);
         }
      }
   }
}

void MainWindow::on_actionMostrar_zoom_triggered() {
   zoomedRegionWindow->show();
   zoomedRegionWindow->parentWidget()->show();
}

void MainWindow::on_actionCalibrate_triggered() {
   bool ok;
   int minHessian = QInputDialog::getInt(this, "Calibration parameters",
                        "Minimum Hessian", 400, 1, INT_MAX, 1, &ok);
   if (ok) {
      cv::Mat calibrationResult =
            getCurrentProject()->performCalibration(minHessian);
      ShowImageDialog *imageDialog =
            new ShowImageDialog("Calibration results",
                                cvMat2QtImage(calibrationResult), this);
      if (imageDialog->exec() == QDialog::Accepted) {
         getCurrentProject()->setGlobalCorrectionMatrices();
         getCurrentProject()->saveCalibrationParameters();
         getCurrentProject()->calculateCorrectedData();
      }
   }
}

} // namespace RatWalkGui
