#include "RatWalkGui/Plotter.h"

#include <cmath>
#include <stdexcept>

#include <QColor>
#include <QPainter>
#include <QPointF>
#include <QSize>
#include <QDebug>

namespace RatWalkGui {

Plotter::Plotter(QWidget *parent)
  : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    xAxisLength = 100;
    yAxisLength = 100;
    xAxisPos    = 20;
    yAxisPos    = 20;
}

void Plotter::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setBrush(QColor("whitesmoke"));
    painter.drawRect(-1, -1, size().width()+1, size().height()+1);
    painter.drawLine(QPointF(0, size().height()-xAxisPos),
                     QPointF(size().width(), size().height()-xAxisPos)); // eje x
    painter.drawLine(QPointF(xAxisPos, 0),
                     QPointF(xAxisPos, size().height())); // eje y

    std::vector<QPointF> translatedPoints = translatePoints();
    painter.drawPolyline(translatedPoints.data(), (int)points.size());
}

void Plotter::addPoint(double x, double y) {
    points.push_back(QPointF(x, y));
    update();
}

void Plotter::clearPoints() {
    points.clear();
    update();
}

int Plotter::getPointCount() {
    return (int)points.size();
}

void Plotter::deletePoint(int idx) {
    if (idx < 0 || idx >= getPointCount()) {
        throw std::out_of_range("idx < 0 || idx >= getPointCount()");
    }
    points.erase(points.begin() + idx);
    update();
}

void Plotter::setXAxisLength(double len) {
    if (std::isnan(len) || len <= 0) {
        throw std::invalid_argument("std::isnan(len) || len <= 0");
    }
    xAxisLength = len;
}

void Plotter::setYAxisLength(double len) {
    if (std::isnan(len) || len <= 0) {
        throw std::invalid_argument("std::isnan(len) || len <= 0");
    }
    yAxisLength = len;
}

std::vector<QPointF> Plotter::translatePoints() {
    std::vector<QPointF> translatedPoints;
    double xOffset = size().width()  - yAxisPos;
    double yOffset = size().height() - xAxisPos;
    for (QPointF &point : points) {
        double newX = point.x() * xOffset / xAxisLength;
        double newY = point.y() * yOffset / yAxisLength;
        newX = newX    + yAxisPos;
        newY = yOffset - newY;
        translatedPoints.push_back(QPointF(newX, newY));
    }
    return translatedPoints;
}

} // namespace RatWalkGui
