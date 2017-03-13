#include "RatWalkGui/Plotter.h"

#include <cmath>
#include <stdexcept>

#include <QColor>
#include <QPainter>
#include <QPointF>
#include <QSize>
#include <QRectF>
#include <QDebug>

namespace RatWalkGui {

Plotter::Plotter(QWidget *parent)
  : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setXRange(-5, 100);
    setYRange(-5, 100);
}

void Plotter::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter(this);
    painter.setBrush(QColor("whitesmoke"));
    painter.drawRect(-1, -1, size().width()+1, size().height()+1);
    painter.setPen(Qt::red);
    painter.drawLine(logicPoint2RealPoint(QPointF(minX, 0)),
                     logicPoint2RealPoint(QPointF(maxX, 0))); // eje x
    painter.setPen(Qt::blue);
    painter.drawLine(logicPoint2RealPoint(QPointF(0, minY)),
                     logicPoint2RealPoint(QPointF(0, maxY))); // eje y

    painter.setPen(Qt::lightGray);
    for (double logicX : verticalLines) {
        painter.drawLine(logicPoint2RealPoint(QPointF(logicX, minY)),
                         logicPoint2RealPoint(QPointF(logicX, maxY)));
    }

    painter.setPen(QColor(255, 0, 0, 100));
    painter.setBrush(QColor(255, 0, 0, 100));
    for (const std::pair<double, double> &rectInfo : verticalRects) {
        QRectF rectangle(logicPoint2RealPoint(QPointF(rectInfo.first,  maxY)),
                        logicPoint2RealPoint(QPointF(rectInfo.second, minY)));
        painter.drawRect(rectangle);
    }

    painter.setPen(Qt::black);
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
    setXRange(0, len);
}

void Plotter::setYAxisLength(double len) {
    if (std::isnan(len) || len <= 0) {
        throw std::invalid_argument("std::isnan(len) || len <= 0");
    }
    setYRange(0, len);
}

void Plotter::setXRange(double minX, double maxX) {
    this->minX = minX;
    this->maxX = maxX;
}

void Plotter::setYRange(double minY, double maxY) {
    this->minY = minY;
    this->maxY = maxY;
}

QPointF Plotter::realPoint2LogicPoint(QPointF realPoint) {
    double logicX = logicXRangeLen() / double(size().width())
                    * realPoint.x() + minX;
    double logicY = logicYRangeLen() / double(size().height())
                    * (size().height() - realPoint.y()) + minY;
    return QPointF(logicX, logicY);
}

QPointF Plotter::logicPoint2RealPoint(QPointF logicPoint) {
    double realX = logicPoint.x() - minX;
           realX = double(size().width())  / logicXRangeLen() * realX;
    double realY = logicPoint.y() - minY;
           realY = double(size().height()) / logicYRangeLen() * realY;
           realY = size().height() - realY;
    return QPointF(realX, realY);
}

std::vector<QPointF> Plotter::translatePoints() {
    std::vector<QPointF> translatedPoints;
    for (QPointF &point : points) {
        translatedPoints.push_back(logicPoint2RealPoint(point));
    }
    return translatedPoints;
}

double Plotter::logicXRangeLen() {
    return maxX - minX;
}

double Plotter::logicYRangeLen() {
    return maxY - minY;
}

void Plotter::addVerticalLine(double pos) {
    verticalLines.push_back(pos);
    update();
}

void Plotter::clearVerticalLines() {
    verticalLines.clear();
    update();
}

void Plotter::addVerticalRect(double xBegin, double xEnd) {
    verticalRects.push_back(std::make_pair(xBegin, xEnd));
    update();
}

void Plotter::clearVerticalRects() {
    verticalRects.clear();
    update();
}


} // namespace RatWalkGui
