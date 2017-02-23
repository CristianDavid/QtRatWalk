#include "AnglePlotter.h"
#include "ui_AnglePlotter.h"

#include <vector>

#include <QSize>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QString>

namespace RatWalkGui {

AnglePlotter::AnglePlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnglePlotter) {
    ui->setupUi(this);
    ui->plotter->installEventFilter(this);
    ui->plotter->setYRange(-5, 360);
}

AnglePlotter::~AnglePlotter() {
    delete ui;
}

void AnglePlotter::setFramesPerVideo(std::vector<int> &framesPerVideo) {
    this->framesPerVideo = framesPerVideo;
    int sum = 0;
    for (int frames : framesPerVideo) {
        sum += frames;
        ui->plotter->addVerticalLine(sum);
    }
    ui->plotter->setXRange(-5, sum);
}

void AnglePlotter::addStep(int video, int beginFrame, int endFrame) {
    int offset = 0;
    for (int i = 0; i < video; i++) {
        offset += framesPerVideo[i];
    }
    ui->plotter->addVerticalRect(offset + beginFrame,
                                 offset + endFrame);
}

void AnglePlotter::clearSteps() {
    ui->plotter->clearVerticalRects();
}

Plotter *AnglePlotter::getPlotter() {
    return ui->plotter;
}

bool AnglePlotter::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->plotter) {
        QMouseEvent *mouseEvent;
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                break;
            case QEvent::MouseButtonRelease:
                break;
            case QEvent::MouseMove:
                mouseEvent = static_cast<QMouseEvent*>(event);
                showCurrentCoordinates(mouseEvent->pos());
                break;
            default:
                ;
                break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void AnglePlotter::on_toolBtnZoomIn_clicked() {
    QSize defaultSize = ui->plotter->parentWidget()->size() - QSize(1, 1);
    ui->plotter->resize(ui->plotter->size() + defaultSize*0.1);
}

void AnglePlotter::on_toolBtnZoomOut_clicked() {
    QSize defaultSize = ui->plotter->parentWidget()->size() - QSize(1, 1);
    ui->plotter->resize(ui->plotter->size() - defaultSize*0.1);
}

void AnglePlotter::on_toolBtnZoomFitBest_clicked() {
    QSize defaultSize = ui->plotter->parentWidget()->size() - QSize(1, 1);
    ui->plotter->resize(defaultSize);
}

void AnglePlotter::showCurrentCoordinates(QPoint realPoint) {
    if (framesPerVideo.size() == 0) return;
    QPointF logicPoint = ui->plotter->realPoint2LogicPoint(realPoint);
    int    globalFrame = logicPoint.x();
    int    localFrame  = localFrame = globalFrame;
    int    videoNumber;
    double angle       = logicPoint.y();
    for (int i = 0; i < (int)framesPerVideo.size(); i++) {
        if (localFrame < framesPerVideo[i]) {
            videoNumber = i;
            break;
        } else {
            localFrame -= framesPerVideo[i];
        }
    }
    ui->edVideoNumber->setText(QString::number(videoNumber));
    ui->edFrame->setText(QString::number(localFrame));
    ui->edAngle->setText(QString::number(angle));
}

} // namespace RatWalkGui
