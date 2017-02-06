#include "RatWalkGui/Plotter.h"

#include <QPainter>
#include <QPointF>
#include <QDebug>

RatWalkGui::Plotter::Plotter(QWidget *parent)
  : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    points = {QPointF(20, 20), QPointF(20, 50), QPointF(100, 100)};
}

void RatWalkGui::Plotter::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    qDebug() << "La vida es mejor pintando";
    painter.setBackground(Qt::red);
    painter.setBrush(Qt::gray);
    painter.drawRect(0, 0, size().width(), size().height());
    painter.drawPolyline(points.data(), (int)points.size());
}
