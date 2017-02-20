#include "AnglePlotter.h"
#include "ui_AnglePlotter.h"

#include <QSize>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

namespace RatWalkGui {

AnglePlotter::AnglePlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnglePlotter) {
    ui->setupUi(this);
    ui->plotter->installEventFilter(this);
}

AnglePlotter::~AnglePlotter() {
    delete ui;
}

Plotter *AnglePlotter::getPlotter() {
    return ui->plotter;
}

bool AnglePlotter::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->plotter) {
        QMouseEvent *mouseEvent;
        switch (event->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
                //mouseMoveEventOnPnlFrame((QMouseEvent*)event);
                mouseEvent = static_cast<QMouseEvent*>(event);
                qDebug() << mouseEvent->pos();
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

} // namespace RatWalkGui
